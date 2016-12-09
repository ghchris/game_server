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
#include <assistx2/json_wrapper.h>
#include "gameconfigdata.h"
#include "cardgenerator.h"
#include "seatdata.h"
#include "scenemanager.h"
#include "common.h"
#include "datalayer.h"
#include "gamedatamanager.h"
#include "logserver.h"

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
const static std::int16_t SERVER_BROADCAST_CHAT = 1010;
const static std::int16_t SERVER_NOTIFY_MSG_LIST = 1011;
const static std::int16_t COMMAND_DISBAND_VOTE = 1012;
const static std::int16_t SERVER_BROADCAST_HAS_BEEN_DISBAND = 1013;

const static std::int16_t NET_CONNECT_CLOSED = 1999;
const static std::int16_t SERVER__UPDATE_GOLD = 1086;

const static std::int32_t ERROR_CODE_SUCCESS = 0; 
const static std::int32_t ERROR_CODE_GAEME_NUM_ENOUGH = -1000; //游戏次数不足
const static std::int32_t ERROR_CODE_ROOM_ALREADY_USED = -1001;//房间已使用不能离开
const static std::int32_t ERROR_CODE_LEAVE_ROOM_OWNNER = -1002;//房主不能离开
const static std::int32_t ERROR_CODE_ALREADY_REQUEST_DIABAND = -1003; //已经发起解散房间

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
    void SendMsgList(std::shared_ptr<Agent > player,std::int32_t begin,std::int32_t num);
    void SendVoteMessage(std::shared_ptr<Agent > player,std::int32_t err);
    void ClearVoteData();
    void RefundGold();
public:
    void OnClientClose(std::shared_ptr<Agent > player);
    void OnReady(std::shared_ptr<Agent > player);
    void OnLeave(std::shared_ptr<Agent > player);
    void OnRequestDisbandRoom(std::shared_ptr<Agent > player);
    void OnChat(std::shared_ptr<Agent > player,assistx2::Stream* packet);
    void OnNotifyChatMessage(std::shared_ptr<Agent > player, assistx2::Stream* packet);
    void OnVote(std::shared_ptr<Agent > player, assistx2::Stream* packet);
public:
    PrivateRoom* owner_ = nullptr;
    std::int32_t banker_seatno_ = Table::INVALID_SEAT;//庄家座位号
    std::int32_t played_num_ = 0;//已完的次数
    std::shared_ptr<CardGenerator> card_generator_;//发牌器
    std::int32_t disband_author_ = Table::INVALID_SEAT;//解散房间发起者座位号
    time_t disband_start_time_ = 0;//解散房间发起时间
    std::vector<std::pair<std::string, std::string>> msg_list_;
    uid_type proxy_uid_ = 0;
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
    case SERVER_BROADCAST_CHAT:
        pImpl_->OnChat(player, packet);
        break;
    case  SERVER_NOTIFY_MSG_LIST:
        pImpl_->OnNotifyChatMessage(player, packet);
        break;
    case COMMAND_DISBAND_VOTE:
        pImpl_->OnVote(player, packet);
        break;
    default:
        break;
    }

    return 0;
}

void PrivateRoom::OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context)
{
    auto ptr = dynamic_cast<EventTimerContext *>(context.get());
    switch (ptr->type_)
    {
    case SceneTimerContext::MAJIANG_DISBAND:
        OnDisbandRoom(DisbandType::TIME_OUT);
       break;
    default:
        break;
    }
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
        res = table_obj()->Enter(player, player->seat_no());
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

void PrivateRoom::set_played_num(std::int32_t num)
{
    pImpl_->played_num_ = num;
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

const std::int32_t PrivateRoom::played_num() const
{
    return pImpl_->played_num_;
}

void PrivateRoom::set_proxy_uid(const uid_type mid)
{
    pImpl_->proxy_uid_ = mid;
}

const uid_type PrivateRoom::proxy_uid() const
{
    return pImpl_->proxy_uid_;
}

void PrivateRoom::OnGameStart()
{
    set_room_state(RoomBase::RoomState::PLAYING);
    DataLayer::getInstance()->set_room_data_to_cache(
        scene_id(), RoomDataToString(true));
}

void PrivateRoom::OnGameOver()
{
    set_room_state(RoomBase::RoomState::WAITING);
    DataLayer::getInstance()->set_room_data_to_cache(
        scene_id(), RoomDataToString(true));

    if (pImpl_->played_num_ >= room_conifg_data()->ju)
    {
        OnDisbandRoom(DisbandType::NUM_NULL);
    }
}

void PrivateRoom::OnDisbandRoom(DisbandType type)
{
    
    if (0 == pImpl_->played_num_)
    {
        pImpl_->RefundGold();
        type = DisbandType::NOT_START;
    }

    assistx2::Stream package(SERVER_BROADCAST_HAS_BEEN_DISBAND);
    package.Write(scene_id());
    package.Write(scene_type());
    package.Write(static_cast<std::int32_t>(type));
    package.End();
    BroadCast(package,nullptr,true);

    auto players = players_agent();
    for (auto iter : players)
    {
        Leave(iter.second);
    }

    set_room_state(RoomBase::RoomState::CLOSED);
    pImpl_->banker_seatno_ = Table::INVALID_SEAT;
    pImpl_->played_num_ = 0;
    pImpl_->disband_author_ = Table::INVALID_SEAT;
    pImpl_->disband_start_time_ = 0;
    pImpl_->msg_list_.clear();
    set_room_owner(0);
    SceneManager::getInstance()->DetachActivedPrivateRoom(this);
    DataLayer::getInstance()->set_room_data_to_cache(scene_id(), std::string());
}

std::int32_t PrivateRoom::Disband()
{
    auto players = players_agent();
    for (auto iter : players)
    {
        Leave(iter.second);
    }

    set_room_state(RoomBase::RoomState::CLOSED);
    pImpl_->banker_seatno_ = Table::INVALID_SEAT;
    pImpl_->played_num_ = 0;
    pImpl_->disband_author_ = Table::INVALID_SEAT;
    pImpl_->disband_start_time_ = 0;
    pImpl_->msg_list_.clear();
    set_room_owner(0);
    SceneManager::getInstance()->DetachActivedPrivateRoom(this);

    return RoomBase::Disband();
}

void PrivateRoom::OnReConect(std::shared_ptr<Agent > player)
{
    if (pImpl_->disband_author_ != Table::INVALID_SEAT)
    {
        pImpl_->SendVoteMessage(player, -1);
    }

    pImpl_->ReConnect(player);
}

void PrivateRoom::StringToRoomData(const std::string& str)
{
    auto value = Common::StringToJson(str);
    auto & obj = value.get_obj();
    for (auto iter : obj)
    {
        if (iter.name_ == "roomowner")
        {
            set_room_owner(assistx2::ToInt32(iter.value_));
            continue;
        }
        if (iter.name_ == "banker")
        {
            pImpl_->banker_seatno_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "playednum")
        {
            pImpl_->played_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "createnum")
        {
            set_create_time(assistx2::ToInt32(iter.value_));
            continue;
        }
        if (iter.name_ == "players")
        {
            auto players = assistx2::ToInt32(iter.value_);
            DCHECK(players <= 4 && players >= 2);
            set_table_obj(std::make_shared<Table>(players));
            continue;
        }
        if (iter.name_ == "proxyuid")
        {
            pImpl_->proxy_uid_ = assistx2::ToInt32(iter.value_);
            continue;
        }
    }
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
        packet.Write(member_fides->city());
        packet.Write(std::string("")); //占位
//         DLOG(INFO) << "RoomSnapShot roomid:=" << owner_->scene_id() << ",mid:=" 
//             << iter.second->uid() << ", seatno:=" << iter.second->seat_no() << ", gp:=" 
//             << member_fides->gp() << ", sex:=" << member_fides->sex() << ", name:=" 
//             << member_fides->name()<< ", icon:=" << member_fides->icon();
    }
    packet.Write(banker_seatno_);
    auto& seats = owner_->table_obj()->GetSeats();
    packet.Write(static_cast<std::int32_t>(seats.size()));
    packet.Write(owner_->RoomDataToString(false));
    packet.End();

    DLOG(INFO) << "RoomSnapShot roomid:=" << owner_->scene_id() << ",room_owner:=" << owner_->room_owner()
        << ",room_state:=" << static_cast<std::int32_t>(owner_->room_state()) << ",played_num_:=" << played_num_
        << ",ju:=" << owner_->room_conifg_data()->ju << ",banker_seatno_:=" << banker_seatno_ << ",max_seats:="
        << seats.size() << ",package:=" << packet.GetNativeStream().GetSize();

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
    packet.Write(member_fides->city());
    packet.Write(std::string("")); //占位

    auto seat_obj = owner_->table_obj()->GetBySeatNo(player->seat_no());
    packet.Write(seat_obj->data()->seat_score_);
    packet.End();

    owner_->BroadCast(packet, player);
}

void PrivateRoomImpl::ReConnect(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    assistx2::Stream packet(SERVER_BROADCAST_PLAYER_RECONNECT);
    packet.Write(seatno);
    packet.End();

    owner_->BroadCast(packet, player);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    auto state = seat->seat_player_state() & ~Seat::PLAYER_STATUS_NET_CLOSE;
    seat->set_seat_player_state(state);
    if ( (state & Seat::PLAYER_STATUS_READY) != 0)
    {
        OnReady(player);
    }
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
    if ( Common::PushServersStopped(player) ) return ;

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
    package.Write(player->seat_no());
    package.End();

    if (err != 0)
    {
        player->SendTo(package);
    }
    else
    {
        owner_->Leave(player);
        owner_->BroadCast(package);
    }
}

void PrivateRoomImpl::OnRequestDisbandRoom(std::shared_ptr<Agent > player)
{
    auto now_players = owner_->table_obj()->player_count();
    auto max_seats = owner_->table_obj()->GetSeats().size();

    if (now_players < static_cast<std::int32_t>(max_seats))
    {
        owner_->OnDisbandRoom(PrivateRoom::DisbandType::ALL_AGREE);
        return;
    }

    auto err = 0;
    if (disband_author_ == Table::INVALID_SEAT)
    {
        disband_author_ = player->seat_no();
        disband_start_time_ = time(nullptr);
#ifdef TEST_TIME
        owner_->NewTimer(10 * 1000, SceneTimerContext::MAJIANG_DISBAND);
#else
        owner_->NewTimer(5*60 * 1000, SceneTimerContext::MAJIANG_DISBAND);
#endif
    }
    else
    {
        err = ERROR_CODE_ALREADY_REQUEST_DIABAND;
    }

    SendVoteMessage(player,err);
}

void PrivateRoomImpl::OnVote(std::shared_ptr<Agent > player, assistx2::Stream* packet)
{
    auto vote = packet->Read<std::int32_t>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);
    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    auto err = 0;
    if (disband_author_ == Table::INVALID_SEAT)
    {
        err = -1;
    }

    if (seat->data()->disband_vote_ != 0)
    {
        err = -2;
    }

    assistx2::Stream stream(COMMAND_DISBAND_VOTE);
    stream.Write(err);
    stream.End();
    player->SendTo(stream);

    if (err != 0)
    {
        return;
    }

    if (vote == 1)
    {
        seat->data()->disband_vote_ = vote;
    }
    else
    {
        seat->data()->disband_vote_ = 2;
    }

    SendVoteMessage(player,0);
    
    auto vote_count_true = 0;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->data()->disband_vote_ == 2)
        {
            ClearVoteData();
            return;
        }
        if (iter->data()->disband_vote_ == 1)
        {
            vote_count_true += 1;
        }
    }

    if (vote_count_true >= static_cast<std::int32_t>(seats.size() - 1))
    {
        ClearVoteData();
        owner_->OnDisbandRoom(PrivateRoom::DisbandType::ALL_AGREE);
    }
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

    owner_->OnGameStart();
}

void PrivateRoomImpl::OnChat(std::shared_ptr<Agent > player, assistx2::Stream* packet)
{
    auto message = packet->Read<std::string>();
    
    auto name = player->member_fides()->name();
  
    msg_list_.push_back(std::make_pair(name, message));

    assistx2::Stream stream(SERVER_BROADCAST_CHAT);
    stream.Write(player->seat_no());
    stream.Write(message);
    stream.Write(static_cast<std::int32_t>(msg_list_.size()));
    stream.Write(name);
    stream.End();

    owner_->BroadCast(stream);
}

void PrivateRoomImpl::OnNotifyChatMessage(std::shared_ptr<Agent > player, assistx2::Stream* packet)
{
    auto begin = packet->Read<std::int32_t>();
    auto size = packet->Read<std::int32_t>();

    if (begin == -1)
    {
        begin = msg_list_.size() - 1;
    }

    SendMsgList(player, begin, size);
}

void PrivateRoomImpl::SendMsgList(std::shared_ptr<Agent > player, std::int32_t begin, std::int32_t size)
{
    auto max_size = static_cast<std::int32_t>(msg_list_.size());
    assistx2::Stream stream(SERVER_NOTIFY_MSG_LIST);
    if (size > max_size)
    {
        stream.Write(max_size);
    }
    else
    {
        stream.Write(static_cast<std::int32_t>(size));
    }
    for (std::int32_t i = begin; i >= (begin - size); --i)
    {
        if (i >= static_cast<std::int32_t>(msg_list_.size()) ||
            i < 0)
        {
            continue;
        }
        stream.Write(i);
        stream.Write(msg_list_[i].first);
        stream.Write(msg_list_[i].second);
    }
    stream.End();

    player->SendTo(stream);
}

void PrivateRoomImpl::SendVoteMessage(std::shared_ptr<Agent > player,std::int32_t err)
{
    auto now_players = owner_->table_obj()->player_count();
#ifdef TEST_TIME
    auto over_time = 10 - (time(nullptr) - disband_start_time_);
#else
    auto over_time = 300 - (time(nullptr) - disband_start_time_);
#endif // TEST_TIME

    
    assistx2::Stream stream(CLEINT_REQUEST_DISBAND_ROOM);
    stream.Write(err);
    stream.Write(disband_author_);
    stream.Write(static_cast<std::int32_t>(over_time));
    stream.Write(now_players - 1);
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->player() == nullptr)
        {
            continue;
        }
        if (iter->seat_no()
            == disband_author_)
        {
            continue;
        }
        
        stream.Write(iter->seat_no());
        stream.Write(iter->data()->disband_vote_);
    }
    stream.End();

    if (err == 0)
    {
        owner_->BroadCast(stream);
    }
    else
    {
        player->SendTo(stream);
    }
}

void PrivateRoomImpl::ClearVoteData()
{
    owner_->CancelTimer(SceneTimerContext::MAJIANG_DISBAND);
    disband_author_ = Table::INVALID_SEAT;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->disband_vote_ = 0;
    }
}

void PrivateRoomImpl::RefundGold()
{
    auto room_owner_mid = owner_->room_owner();

    if (owner_->proxy_uid() != 0)
    {
        std::int64_t amount = 0;
        std::int64_t real_pay = 0;
        auto err = DataLayer::getInstance()->Pay(owner_->proxy_uid(), 
            -owner_->room_conifg_data()->cost, amount, real_pay, false);
        if (err == 0)
        {
            auto proxy_player = owner_->watchdog_obj()->GetAgentByID(owner_->proxy_uid());
            if (proxy_player != nullptr)
            {
                assistx2::Stream stream(SERVER__UPDATE_GOLD);
                stream.Write(amount);
                stream.End();
                proxy_player->SendTo(stream);
            }

            LogServer::getInstance()->WriteGoldLog(owner_->proxy_uid(), 
                owner_->room_conifg_data()->cost, amount, 5, room_owner_mid);

            LOG(INFO) << "RefundGold " << ", proxy_mid:=" << owner_->proxy_uid() << ",delta:" 
                << owner_->room_conifg_data()->cost << ",amount:" << amount;
        }
        else
        {
            LOG(ERROR) << "RefundGold FALIED, err:=" << err << ", proxy_mid:=" << owner_->proxy_uid() 
                << ", gold:=" << owner_->room_conifg_data()->cost;
        }
    }
    else
    {
        auto players = owner_->players_agent();
        auto iter = players.find(room_owner_mid);
        if (iter != players.end())
        {
            iter->second->GoldPay(-owner_->room_conifg_data()->cost, 5);
        }
        else
        {
            LOG(ERROR) << "DisbandRoom: room_owner not in scene mid:=" << room_owner_mid;
        }
    }
}