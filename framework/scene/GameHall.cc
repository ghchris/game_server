#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include <assistx2/string_wrapper.h>
#include "watchdog.h"
#include "playeragent.h"
#include "GameHall.h"
#include "scenemanager.h"
#include "hzmajiangroom.h"
#include "zhuanzhuanroom.h"
#include "changsharoom.h"
#include "gameconfigdata.h"
#include "robotmanager.h"
#include "timerhelper.h"
#include "table.h"
#include "common.h"
#include "datalayer.h"
#include "logserver.h"

const static std::int16_t SERVER__UPDATE_GOLD = 1086;
const static std::int16_t NET_CONNECT_CLOSED = 1999;

const static std::int16_t SERVER_RESPONSE_ENTER_GAMEHALL = 1100;
const static std::int16_t SERVER_RESPONSE_LEAVE_GAMEHALL = 1101;

const static std::int16_t CLEINT_REQUEST_CREATE_ROOM = 1102; //创建房间
const static std::int16_t CLEINT_REQUEST_ATTACH_ROOM = 1103;//加入房间

const static std::int32_t ERROR_CODE_ROOM_NOT_ENOUGH = -1100; //房间不足
const static std::int32_t ERROR_CODE_GOLD_NOT_ENOUGH = -1101; //金币不足
const static std::int32_t ERROR_CODE_ROOM_NOT_FOUND = -1102; //房间未找到
const static std::int32_t ERROR_CODE_NOT_HAVE_PROXY= -1103; //未指定代理
const static std::int32_t ERROR_CODE_PROXY_GOLD_NOT_ENOUGH = -1104; //代理金币不足

class GameHallImpl
{
public:
    GameHallImpl(GameHall* owner);
    ~GameHallImpl();
    void OnClientClose(std::shared_ptr<Agent > player);
    void OnCreateRoom(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnEnterRoom(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void SendErrorCode(std::shared_ptr<Agent > player, const std::int16_t cmd,const std::int32_t err);
    bool SetRoomParam(std::shared_ptr<Agent > player, const std::string& type,RoomBase* room, assistx2::Stream * packet);
    std::int32_t GoldPlay(std::shared_ptr<Agent > player, std::int32_t cost,std::int32_t isproxy);
public:
    GameHall* owner_;
};

GameHall::GameHall() :
    SceneBase(0, "GH"),
    pImpl_(new GameHallImpl(this))
{

}

GameHall::~GameHall()
{

}

std::int32_t GameHall::Enter(std::shared_ptr<Agent > player)
{
    assistx2::Stream stream(SERVER_RESPONSE_ENTER_GAMEHALL);
    stream.End();

    player->SendTo(stream);

    return SceneBase::Enter(player);
}

std::int32_t GameHall::Leave(std::shared_ptr<Agent > player)
{
    assistx2::Stream stream(SERVER_RESPONSE_LEAVE_GAMEHALL);
    stream.End();

    player->SendTo(stream);

    return SceneBase::Leave(player);
}

std::int32_t GameHall::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    if (Common::PushServersStopped(player)) return 0;

    auto cmd = packet->GetCmd();
    DLOG(INFO) << "GameHall::OnMessage()->cmd:" << cmd << " Scene:=" << scene_id()
        << " Scene Type:=" << scene_type() << " mid:=" << player->uid();

    switch (cmd)
    {
    case NET_CONNECT_CLOSED:
        pImpl_->OnClientClose(player);
        return 0;
    case CLEINT_REQUEST_CREATE_ROOM:
        pImpl_->OnCreateRoom(player,packet);
        return 0;
    case CLEINT_REQUEST_ATTACH_ROOM:
        pImpl_->OnEnterRoom(player, packet);
        return 0;
    default:
        break;
    }

    return 0;
}

GameHallImpl::GameHallImpl(GameHall* owner) :
    owner_(owner)
{

}

GameHallImpl::~GameHallImpl()
{
}

void GameHallImpl::OnCreateRoom(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    const std::string type = packet->Read<std::string>();
    const std::int32_t ju = packet->Read<std::int32_t>();

    auto room = SceneManager::getInstance()->GetRoomByType(type, ju);
    if (room == nullptr)
    {
        SendErrorCode(player, CLEINT_REQUEST_CREATE_ROOM, ERROR_CODE_ROOM_NOT_ENOUGH);
        return;
    }
   
    auto err = SetRoomParam(player,type,room, packet);
    if (err == false) return;

    room->set_room_owner(player->uid());
    room->set_create_time(time(nullptr));

    auto res = room->Enter(player);
    if (res <= 0)
    {
        LOG(ERROR) << "GameHallImpl::OnCreateRoom: Faild res:=" << res << " mid:=" << player->uid()
            << " roomid:=" << room->scene_id();
        //进入房间失败 退还金币
        player->GoldPay(-room->room_conifg_data()->cost, 5);
        room->set_room_owner(0);
        return;
    }
    else
    {
        owner_->Leave(player);
        player->set_scene_object(room);
    }
    room->set_room_state(RoomBase::RoomState::WAITING);
    SceneManager::getInstance()->AttachActivedPrivateRoom(room);

    DataLayer::getInstance()->set_room_data_to_cache(
        room->room_index(), room->RoomDataToString(true));

    GlobalTimerProxy::getInstance()->NewTimer(
        std::bind(&RobotManager::AttachRobot, RobotManager::getInstance(), room), 1);
}

void GameHallImpl::OnEnterRoom(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    const std::int32_t roomid = packet->Read<std::int32_t>();

    auto room = SceneManager::getInstance()->GetRoomFromActivedPrivateRoom(roomid);
    if (room == nullptr)
    {
        SendErrorCode(player, CLEINT_REQUEST_ATTACH_ROOM, ERROR_CODE_ROOM_NOT_FOUND);
        return;
    }
    auto res = room->Enter(player);
    if (res <= 0)
    {
        LOG(ERROR) << "GameHallImpl::OnEnterRoom: Faild res:=" << res << " mid:=" << player->uid()
            << " roomid:=" << room->scene_id();
        return;
    }
    else
    {
        owner_->Leave(player);
        player->set_scene_object(room);
    }
}

void GameHallImpl::OnClientClose(std::shared_ptr<Agent > player)
{
    owner_->Leave(player);
    player->set_scene_object(nullptr);
    player->set_connect_status(false);
    player->watch_dog()->RemoveAgent(player->uid());
}

void GameHallImpl::SendErrorCode(std::shared_ptr<Agent > player, 
    const std::int16_t cmd, const std::int32_t err)
{
    assistx2::Stream stream(cmd);
    stream.Write(err);
    stream.End();

    player->SendTo(stream);
}

bool GameHallImpl::SetRoomParam(std::shared_ptr<Agent > player, const std::string& type, RoomBase* room, assistx2::Stream * packet)
{
    const std::int32_t players = packet->Read<std::int32_t>();
    const std::int32_t play_type = packet->Read<std::int32_t>();
    const std::int32_t operation = packet->Read<std::int32_t>();
    const std::int32_t zhuaniao = packet->Read<std::int32_t>();
    const std::int32_t isproxy = packet->Read<std::int32_t>();

    LOG(INFO) << "GameHallImpl::SetRoomParam: roomid:="<< room->scene_id() << ",players:=" 
        << players << ",play_type:=" << play_type << ",operation:=" << operation << ",zhuaniao:=" << zhuaniao;

    auto proxy_uid = GoldPlay(player, room->room_conifg_data()->cost, isproxy);
    if (proxy_uid < 0)
    {
        SendErrorCode(player, CLEINT_REQUEST_CREATE_ROOM, proxy_uid);
        return false;
    }

    if (type == "1")
    {
        auto hzroom = dynamic_cast<HzMajiangRoom*>(room);
        hzroom->set_table_obj(std::make_shared<Table>(4));
        hzroom->set_operation(operation);
        hzroom->set_zhama_num(zhuaniao);
        hzroom->set_proxy_uid(proxy_uid);
    }
    else if (type == "2")
    {
        auto zzroom = dynamic_cast<ZhuanZhuanRoom*>(room);
        zzroom->set_table_obj(std::make_shared<Table>(players));
        zzroom->set_operation(operation);
        zzroom->set_zhama_num(zhuaniao);
        zzroom->set_playtype(play_type);
        zzroom->set_playlogic(players);
        zzroom->set_proxy_uid(proxy_uid);
    }
    else if (type == "3")
    {
        auto csroom = dynamic_cast<ChangShaRoom*>(room);
        csroom->set_table_obj(std::make_shared<Table>(players));
        csroom->set_operation(operation);
        csroom->set_zhama_num(zhuaniao);
        csroom->set_proxy_uid(proxy_uid);
    }

    return true;
}

std::int32_t GameHallImpl::GoldPlay(std::shared_ptr<Agent > player, std::int32_t cost, std::int32_t isproxy)
{
    auto proxy_uid = 0;
    if (isproxy == 1)
    {
        std::string data;
        auto res = DataLayer::getInstance()->proxy_mid(player->uid(), data);
        if (res == false) return ERROR_CODE_NOT_HAVE_PROXY;

        proxy_uid = assistx2::atoi_s(data);
        if (proxy_uid == 0) return ERROR_CODE_NOT_HAVE_PROXY;
        
        std::int64_t amount = 0;
        std::int64_t real_pay = 0;
        auto err = DataLayer::getInstance()->Pay(proxy_uid, cost, amount, real_pay, false);
        if (err != 0) return ERROR_CODE_PROXY_GOLD_NOT_ENOUGH;

        auto proxy_player = owner_->watchdog_obj()->GetAgentByID(proxy_uid);
        if (proxy_player != nullptr)
        {
            assistx2::Stream stream(SERVER__UPDATE_GOLD);
            stream.Write(amount);
            stream.End();
            proxy_player->SendTo(stream);
        }
        
        LogServer::getInstance()->WriteGoldLog(proxy_uid, -cost, amount, 4, player->uid());
        LOG(INFO) << "GoldPay " << ", proxy_mid:=" << proxy_uid << ",delta:" << cost << ",amount:" << amount;
    }
    else
    {
        auto res = player->GoldPay(cost, 4);
        if (res == false) return ERROR_CODE_GOLD_NOT_ENOUGH;
    }

    return proxy_uid;
}