#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include "watchdog.h"
#include "playeragent.h"
#include "GameHall.h"
#include "scenemanager.h"
#include "hzmajiangroom.h"
#include "gameconfigdata.h"
#include "table.h"

const static std::int16_t NET_CONNECT_CLOSED = 1999;

const static std::int16_t SERVER_RESPONSE_ENTER_GAMEHALL = 1100;
const static std::int16_t SERVER_RESPONSE_LEAVE_GAMEHALL = 1101;

const static std::int16_t CLEINT_REQUEST_CREATE_ROOM = 1102; //创建房间
const static std::int16_t CLEINT_REQUEST_ATTACH_ROOM = 1103;//加入房间

const static std::int32_t ERROR_CODE_ROOM_NOT_ENOUGH = -1100; //房间不足
const static std::int32_t ERROR_CODE_GOLD_NOT_ENOUGH = -1101; //金币不足
const static std::int32_t ERROR_CODE_ROOM_NOT_FOUND = -1102; //房间未找到

class GameHallImpl
{
public:
    GameHallImpl(GameHall* owner);
    ~GameHallImpl();
    void OnClientClose(std::shared_ptr<Agent > player);
    void OnCreateRoom(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnEnterRoom(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void SendErrorCode(std::shared_ptr<Agent > player, const std::int16_t cmd,const std::int32_t err);
    void SetRoomParam(const std::string& type,RoomBase* room, assistx2::Stream * packet);
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
    //扣除金币
    if ( !player->GoldPay(room->room_conifg_data()->cost,4) )
    {
        SendErrorCode(player, CLEINT_REQUEST_CREATE_ROOM, ERROR_CODE_GOLD_NOT_ENOUGH);
        return;
    }

    SetRoomParam(type,room, packet);
    
    room->set_room_owner(player->uid());
    auto res = room->Enter(player);
    if (res <= 0)
    {
        DLOG(ERROR) << "GameHallImpl::OnCreateRoom: Faild res:=" << res << " mid:=" << player->uid()
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
    room->set_create_time(time(nullptr));
    room->set_room_state(RoomBase::RoomState::WAITING);
    SceneManager::getInstance()->AttachActivedPrivateRoom(room);
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
        DLOG(ERROR) << "GameHallImpl::OnEnterRoom: Faild res:=" << res << " mid:=" << player->uid()
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

void GameHallImpl::SetRoomParam(const std::string& type, RoomBase* room, assistx2::Stream * packet)
{
    const std::int32_t players = packet->Read<std::int32_t>();
    const std::int32_t playe_type = packet->Read<std::int32_t>();
    const std::int32_t operation = packet->Read<std::int32_t>();
    const std::int32_t zhuaniao = packet->Read<std::int32_t>();
    DLOG(ERROR) << "GameHallImpl::SetRoomParam: roomid:="<< room->scene_id() << "players:=" 
        << players << ",playe_type:=" << playe_type<< ",operation:=" << operation << ",zhuaniao" << zhuaniao;

    if (type == "1")
    {
        auto hzroom = dynamic_cast<HzMajiangRoom*>(room);
        hzroom->set_table_obj(std::make_shared<Table>(players));
        if (operation == 1)
        {
            hzroom->set_support_7dui(true);
        }
        hzroom->set_zhama_num(zhuaniao);
    }
}