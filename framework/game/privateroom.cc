#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "privateroom.h"
#include "table.h"
#include "playeragent.h"
#include "memberfides.pb.h"
#include "seat.h"
#include "watchdog.h"
#include "scenemanager.h"
#include <assistx2/tcphandler_wrapper.h>
#include "gameconfigdata.h"
#include "cardgenerator.h"
#include "seatdata.h"
#include "scenemanager.h"

const static std::int16_t SERVER_RESPONSE_ENTER_ROOM = 1001;
const static std::int16_t SERVER_PUSH_PLAYERS_SNAPSHOT = 1002;
const static std::int16_t SERVER_PUSH_TABLE_SNAPSHOT = 1003;
const static std::int16_t SERVER_BROADCAST_PLAYER_INFO = 1004;
const static std::int16_t CLIENT_READY_CMD = 1005;
const static std::int16_t SERVER_BROADCAST_PLAYER_RECONNECT = 1006;
const static std::int16_t SERVER_BROADCAST_GAME_START = 1007;
const static std::int16_t CLEINT_REQUEST_DISBAND_ROOM = 1008;
const static std::int16_t SERVER_BROADCAST_DISBAND_ROOM = 1008;
const static std::int16_t CLEINT_REQUEST_LEAVE_ROOM = 1009;
const static std::int16_t SERVER_RESPONSE_LEAVE_ROOM = 1009;

const static std::int16_t NET_CONNECT_CLOSED = 1999;

const static std::int32_t ERROR_CODE_SUCCESS = 0; 
const static std::int32_t ERROR_CODE_GAEME_NUM_ENOUGH = -1000; //游戏次数不足
const static std::int32_t ERROR_CODE_ROOM_ALREADY_USED = -1001;//房间已使用不能离开
const static std::int32_t ERROR_CODE_LEAVE_ROOM_OWNNER = -1002;//房主不能离开
class PrivateRoomImpl
{
public:
    explicit PrivateRoomImpl(PrivateRoom* owner);
    ~PrivateRoomImpl();
    void ResponseEnterRoom(std::shared_ptr<Agent > player,std::int32_t err);
    void RoomSnapShot(std::shared_ptr<Agent > player);
    void TableSnapShot(std::shared_ptr<Agent > player);
    void BroadCastOnEnterPlayer(std::shared_ptr<Agent > player);
    void ReConnect(std::shared_ptr<Agent > player);
    void StartGame();
    
public:
    void OnClientClose(std::shared_ptr<Agent > player);
    void OnReady(std::shared_ptr<Agent > player);
    void OnLeave(std::shared_ptr<Agent > player);
    void OnRequestDisbandRoom(std::shared_ptr<Agent > player);
public:
    PrivateRoom* owner_ = nullptr;
    std::int32_t banker_seatno_ = Table::INVALID_SEAT;//庄家座位号
    std::int32_t played_num_ = 0;//已完的次数
    std::shared_ptr<CardGenerator> card_generator_;//发牌器
    std::int32_t disband_author_ = 0;//解散房间发起者mid
    time_t disband_start_time_ = 0;//解散房间发起时间
};

PrivateRoom::PrivateRoom(std::uint32_t id, std::string type) :
 RoomBase(id, type),
 pImpl_(new PrivateRoomImpl(this))
{
    pImpl_->card_generator_ = std::make_shared<CardGenerator>();
}

PrivateRoom::~PrivateRoom()
{
   
}

std::int32_t PrivateRoom::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto cmd = packet->GetCmd();
    //DLOG(INFO) << "PrivateRoom::OnMessage()->cmd:" << cmd << " Scene:=" << scene_id()
    //    << " Scene Type:=" << scene_type() << " mid:=" << player->uid();

    switch (cmd)
    {
    case NET_CONNECT_CLOSED:
        pImpl_->OnClientClose(player);
        break;
    case CLIENT_READY_CMD:
        pImpl_->OnReady(player);
        break;
    case CLEINT_REQUEST_DISBAND_ROOM:
        pImpl_->OnRequestDisbandRoom(player);
        break;
    case CLEINT_REQUEST_LEAVE_ROOM:
        pImpl_->OnLeave(player);
        break;
    default:
        break;
    }

    return 0;
}

std::int32_t PrivateRoom::Enter(std::shared_ptr<Agent > player)
{
    auto uid = player->uid();

    auto res = 0;
    auto reconnect = true;

    const auto& players = players_agent();

    auto iter = players.find(uid);
    if (iter == players.end())
    {
        reconnect = false;

        DCHECK(table_obj() != nullptr);
        res = table_obj()->Enter(player);
        if (res > 0)
        {
            RoomBase::Enter(player);
        }
    }
    else
    {
        res = iter->second->seat_no();
    }

    pImpl_->ResponseEnterRoom(player, res);

    if (res <= 0)
    {
        return res;
    }

    pImpl_->RoomSnapShot(player);

    if (reconnect ==  false)
    {
        pImpl_->BroadCastOnEnterPlayer(player);
    }
    else
    {
        OnReConect(player);
    }

    pImpl_->TableSnapShot(player);

    return res;
}

std::int32_t PrivateRoom::Leave(std::shared_ptr<Agent > player)
{
    table_obj()->Leave(player);

    RoomBase::Leave(player);

    if (player->connect_status() == false)
    {
        player->set_scene_object(nullptr);
        player->watch_dog()->RemoveAgent(player->uid());
    }
    else
    {
        auto next_scene = SceneManager::getInstance()->default_scene();
        auto res = next_scene->Enter(player);
        if (res == 0)
        {
            player->set_scene_object(next_scene);
        }
        else
        {
            player->set_scene_object(nullptr);
        }
    }

    return 0;
}

std::shared_ptr<CardGenerator> PrivateRoom::card_generator()
{
    return pImpl_->card_generator_;
}

const std::int32_t PrivateRoom::banker_seatno() const
{
    return pImpl_->banker_seatno_;
}

void PrivateRoom::set_banker_seatno(const std::int32_t seatno)
{
    pImpl_->banker_seatno_ = seatno;
}

void PrivateRoom::OnGameStart()
{
    set_room_state(RoomBase::RoomState::PLAYING);
}

void PrivateRoom::OnGameOver(HuType type)
{
    set_room_state(RoomBase::RoomState::WAITING);

    if (pImpl_->played_num_ >= room_conifg_data()->ju)
    {
        OnDisbandRoom();
    }
}

void PrivateRoom::OnDisbandRoom()
{
    auto players = players_agent();
    if (0 == pImpl_->played_num_)
    {
        auto room_owner_mid = room_owner();
        auto iter = players.find(room_owner_mid);
        if (iter != players.end())
        {
            iter->second->GoldPay(-room_conifg_data()->cost, 5);
        }
        else
        {
            DLOG(ERROR) << "DisbandRoom: room_owner not in scene mid:=" << room_owner_mid;
        }
    }

    assistx2::Stream package(SERVER_BROADCAST_DISBAND_ROOM);
    package.End();
    BroadCast(package);

    for (auto iter : players)
    {
        Leave(iter.second);
    }

    set_room_state(RoomBase::RoomState::CLOSED);
    pImpl_->banker_seatno_ = Table::INVALID_SEAT;
    pImpl_->played_num_ = 0;
    pImpl_->disband_author_ = 0;
    pImpl_->disband_start_time_ = 0;
    set_room_owner(0);
    SceneManager::getInstance()->DetachActivedPrivateRoom(this);
}

void PrivateRoom::OnReConect(std::shared_ptr<Agent > player)
{
    pImpl_->ReConnect(player);
}

PrivateRoomImpl::PrivateRoomImpl(PrivateRoom* owner):
 owner_(owner)
{
}

PrivateRoomImpl::~PrivateRoomImpl()
{
}

void PrivateRoomImpl::ResponseEnterRoom(std::shared_ptr<Agent > player, std::int32_t err)
{
    assistx2::Stream response(SERVER_RESPONSE_ENTER_ROOM);
    response.Write(err);
    response.Write(static_cast<std::int32_t>(owner_->scene_id()));
    response.Write(owner_->scene_type());
    response.End();

    player->SendTo(response);
}

void PrivateRoomImpl::RoomSnapShot(std::shared_ptr<Agent > player)
{
    const auto& players = owner_->players_agent();

    if (banker_seatno_ == Table::INVALID_SEAT)
    {
        auto seat = owner_->table_obj()->GetByUid(owner_->room_owner());
        DCHECK(seat != nullptr);
        banker_seatno_ = seat->seat_no();
    }

    assistx2::Stream packet(SERVER_PUSH_PLAYERS_SNAPSHOT);
    packet.Write(static_cast<std::int32_t>(owner_->room_owner()));
    packet.Write(static_cast<std::int32_t>(owner_->room_state()));
    packet.Write(played_num_);
    packet.Write(owner_->room_conifg_data()->ju);
    packet.Write(static_cast<std::int32_t>(players.size()));
    for (auto iter : players)
    {
        auto member_fides = iter.second->member_fides();
        packet.Write(iter.second->seat_no());
        packet.Write(iter.second->ip_addr());
        packet.Write(member_fides->mid());
        packet.Write(member_fides->gp());
        packet.Write(member_fides->sex());
        packet.Write(member_fides->name());
        packet.Write(member_fides->icon());

//         DLOG(INFO) << "RoomSnapShot roomid:=" << owner_->scene_id() << ",mid:=" 
//             << iter.second->uid() << ", seatno:=" << iter.second->seat_no() << ", gp:=" 
//             << member_fides->gp() << ", sex:=" << member_fides->sex() << ", name:=" 
//             << member_fides->name()<< ", icon:=" << member_fides->icon();
    }
    packet.Write(banker_seatno_);
    auto& seats = owner_->table_obj()->GetSeats();
    packet.Write(static_cast<std::int32_t>(seats.size()));
    packet.End();

    DLOG(INFO) << "RoomSnapShot roomid:=" << owner_->scene_id() << ",room_owner:=" << owner_->room_owner()
        << ",room_state:=" << static_cast<std::int32_t>(owner_->room_state()) << ",played_num_:=" << played_num_
        << ",ju:=" << owner_->room_conifg_data()->ju << ",banker_seatno_:=" << banker_seatno_ << ",max_seats:="
        << seats.size();

    player->SendTo(packet);
}

void PrivateRoomImpl::TableSnapShot(std::shared_ptr<Agent > player)
{
    auto& seats = owner_->table_obj()->GetSeats();
    assistx2::Stream packet(SERVER_PUSH_TABLE_SNAPSHOT);
    packet.Write(owner_->table_obj()->player_count());
    for (auto iter : seats)
    {
        if (iter->player() != nullptr)
        {
            packet.Write(iter->seat_no());
            packet.Write(static_cast<std::int32_t>(iter->seat_player_state()));
            packet.Write(iter->data()->seat_score_);

            DLOG(INFO) << "TableSnapShot(): roomid:=" << owner_->scene_id() << ",mid:=" 
                << iter->player()->uid() << ",seatno:="<< iter->seat_no() << ",seat_player_state:=" 
                << iter->seat_player_state() << ",room_score_:" << iter->data()->seat_score_;
        }
    }
    packet.End();

    player->SendTo(packet);
}

void PrivateRoomImpl::BroadCastOnEnterPlayer(std::shared_ptr<Agent > player)
{
    auto member_fides = player->member_fides();

    assistx2::Stream packet(SERVER_BROADCAST_PLAYER_INFO);
    packet.Write(player->seat_no());
    packet.Write(player->ip_addr());
    packet.Write(member_fides->mid());
    packet.Write(member_fides->gp());
    packet.Write(member_fides->sex());
    packet.Write(member_fides->name());
    packet.Write(member_fides->icon());

    auto seat_obj = owner_->table_obj()->GetBySeatNo(player->seat_no());
    packet.Write(seat_obj->data()->seat_score_);
    packet.End();

    owner_->BroadCast(packet, player);
}

void PrivateRoomImpl::ReConnect(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    auto state = seat->seat_player_state() & ~Seat::PLAYER_STATUS_NET_CLOSE;
    seat->set_seat_player_state(state);
    if ( (state & Seat::PLAYER_STATUS_READY) != 0)
    {
        OnReady(player);
    }

    assistx2::Stream packet(SERVER_BROADCAST_PLAYER_RECONNECT);
    packet.Write(seatno);
    packet.End();

    owner_->BroadCast(packet, player);
}

void PrivateRoomImpl::OnClientClose(std::shared_ptr<Agent > player)
{
    player->set_connect_status(false);

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);

    auto state = seat->seat_player_state() | Seat::PLAYER_STATUS_NET_CLOSE;
    seat->set_seat_player_state(state);
    
    assistx2::Stream package(NET_CONNECT_CLOSED);
    package.Write(seatno);
    package.End();

    owner_->BroadCast(package, player);
}

void PrivateRoomImpl::OnReady(std::shared_ptr<Agent > player)
{
    if (owner_->room_state() ==
        RoomBase::RoomState::PLAYING)
    {
        return;
    }

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    auto state = Seat::PLAYER_STATUS_READY;
    seat->set_seat_player_state(state);

    assistx2::Stream package(CLIENT_READY_CMD);
    package.Write(seatno);
    package.End();

    owner_->BroadCast(package);

    StartGame();
}

void PrivateRoomImpl::OnLeave(std::shared_ptr<Agent > player)
{
    auto err = 0;
    if (player->uid() == owner_->room_owner())
    {
        err = ERROR_CODE_LEAVE_ROOM_OWNNER;
    }

    if (played_num_ != 0 ||
        owner_->room_state() == RoomBase::RoomState::PLAYING)
    {
        err = ERROR_CODE_ROOM_ALREADY_USED;
    }

    assistx2::Stream package(SERVER_RESPONSE_LEAVE_ROOM);
    package.Write(err);
    package.End();

    if (err != 0)
    {
        player->SendTo(package);
    }
    else
    {
        owner_->Leave(player);
        owner_->BroadCast(package, player);
    }
}

void PrivateRoomImpl::OnRequestDisbandRoom(std::shared_ptr<Agent > player)
{
    auto now_players = owner_->table_obj()->player_count();
    auto max_seats = owner_->table_obj()->GetSeats().size();
    if (now_players < static_cast<std::int32_t>(max_seats));
    {
        owner_->OnDisbandRoom();
        return;
    }

    if (disband_author_ == 0)
    {
        disband_author_ = player->uid();
        disband_start_time_ = time(nullptr);
    }

    owner_->OnDisbandRoom();
}

void PrivateRoomImpl::StartGame()
{
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->seat_state() == Seat::SeatState::EMPTY)
        {
            return;
        }
        if (iter->seat_player_state() !=
            Seat::PLAYER_STATUS_READY)
        {
            return;
        }
    }

    played_num_ += 1;

    DLOG(INFO) << "StartGame roomid:=" << owner_->scene_id() << ",ju:=" 
        << owner_->room_conifg_data()->ju << ",played_num_:=" << played_num_;

    auto err = ERROR_CODE_SUCCESS;
    if (owner_->room_conifg_data()->ju - played_num_ < 0)
    {
        err = ERROR_CODE_GAEME_NUM_ENOUGH;
    }

    assistx2::Stream package(SERVER_BROADCAST_GAME_START);
    package.Write(err);
    package.Write(played_num_);
    package.End();
    owner_->BroadCast(package);

    if (err != ERROR_CODE_SUCCESS)
    {
        return;
    }

    auto type = static_cast<CardGenerator::Type>(owner_->room_conifg_data()->type[0] - 0x30);

    DLOG(INFO) << "StartGame: roomid:=" << owner_->scene_id() << 
        ",type:=" << static_cast<std::int32_t>(type);

    card_generator_->Reset(type);

    owner_->OnGameStart();
}
