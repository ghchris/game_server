#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "logserver.h"
#include "privateroom.h"
#include "table.h"
#include "seatdata.h"
#include "seat.h"
#include "agent.h"
#include <assistx2/tcphandler_wrapper.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <assistx2/string_wrapper.h>
#include "gamedatamanager.h"
#include "timerhelper.h"
#include "gameconfigdata.h"

static const std::int16_t XLOGGER_GOLD_RECORD = 9032;
const static std::int16_t XLOGGER_ROOM_RECORD = 9033;
const static std::int16_t XLOGGER_PLAYING_RECORD = 9034;
const static std::int16_t XLOGGER_PLAYER_RECORD = 9035;
const static std::int16_t XLOGGER_MAJINAG_GAME_RECORD = 9100;
const static std::int16_t XLOGGER_MAJINAG_GAME_RECORD_SUB = 9101;
const static std::int16_t XLOGGER_PLAYER_ONLINE = 9040;

class LogServerImpl
{
public:
    LogServerImpl();
    ~LogServerImpl();
    void WritePlayerRecord(uid_type mid,std::int32_t roomid,std::int32_t seatno,std::string uuid);
    void WriteRoomRecord(std::int32_t roomid,std::string uuid);
    void StartOnlineLogTimer();
    void WriteOnlineLog();
public:
    std::shared_ptr<assistx2::TcpHanlderWrapper> connector_;
    std::int32_t game_type_ = 3;
};

LogServer::LogServer():
 pImpl_(new LogServerImpl)
{
}

LogServer::~LogServer()
{
}

bool LogServer::Initialize(std::shared_ptr<assistx2::TcpHanlderWrapper> connector)
{
    pImpl_->connector_ = connector;

    pImpl_->StartOnlineLogTimer();

    return true;
}

void LogServer::WriteGoldLog(const uid_type mid, const std::int64_t gold_incr,
    const  std::int64_t now_gold, const std::int32_t type, const  std::int32_t target)
{
    assistx2::Stream stream(STANDARD_ROUTE_PACKET);
    stream.Write(static_cast<boost::int32_t>(SESSION_TYPE_LOGGER));
    stream.Write(0);
    stream.Write(XLOGGER_GOLD_RECORD);
    stream.Write(mid);
    stream.Write(type); //type
    stream.Write(gold_incr);
    stream.Write(now_gold);
    stream.Write(0); //action
    stream.Write(pImpl_->game_type_); //gametype
    stream.Write(target);

    stream.End();

    pImpl_->connector_->SendTo(stream.GetNativeStream());
}

void LogServer::WriteSubGameLog(PrivateRoom* room, std::int32_t played_num, std::vector<std::int32_t> winner)
{
    boost::uuids::random_generator  generator;

    std::stringstream uuid;
    uuid << generator();

    std::stringstream id;
    id << room->scene_id() << "_" << room->create_time();

    std::stringstream data_game;
    auto& seats = room->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->player() == nullptr)
        {
            continue;
        }
        data_game << iter->player()->uid() << "," 
            << iter->data()->game_score_ << ","
            << iter->data()->seat_score_  << ";";
        pImpl_->WritePlayerRecord(iter->player()->uid(), room->scene_id(), 
            iter->seat_no(), uuid.str());
    }
    std::stringstream ss_winner;
    for (auto iter : winner)
    {
        ss_winner << iter << ";";
    }
    assistx2::Stream stream(STANDARD_ROUTE_PACKET);
    stream.Write(static_cast<boost::int32_t>(SESSION_TYPE_LOGGER));
    stream.Write(0);
    stream.Write(XLOGGER_MAJINAG_GAME_RECORD_SUB);
    stream.Write(id.str());
    stream.Write(played_num);
    stream.Write(data_game.str());
    stream.Write(ss_winner.str());
    stream.Write(uuid.str());
    stream.End();

    pImpl_->connector_->SendTo(stream.GetNativeStream());

    pImpl_->WriteRoomRecord(room->scene_id(), uuid.str());
}

void LogServer::WriteGameLog(PrivateRoom* room, std::int32_t played_num)
{
    if (played_num == 0)
    {
        return;
    }
    std::stringstream id;
    id << room->scene_id() << "_" << room->create_time();

    assistx2::Stream stream(STANDARD_ROUTE_PACKET);
    stream.Write(static_cast<boost::int32_t>(SESSION_TYPE_LOGGER));
    stream.Write(0);
    stream.Write(XLOGGER_MAJINAG_GAME_RECORD);
    stream.Write(room->scene_id());
    stream.Write(played_num);

    std::stringstream data_game;
    auto& seats = room->table_obj()->GetSeats();
    auto size = 0;
    for (auto iter : seats)
    {
        if (iter->player() == nullptr)
        {
            continue;
        }
        stream.Write(static_cast<std::int32_t>(iter->player()->uid()));
        data_game << iter->data()->seat_score_ << ";";
        size += 1;
    }
    for (auto i = size; i < 4; ++i)
    {
        stream.Write(static_cast<std::int32_t>(0));
    }
    stream.Write(data_game.str());
    stream.Write(id.str());
    if (room->proxy_uid() == 0)
    {
        stream.Write(room->room_owner());
    }
    else
    {
        stream.Write(room->proxy_uid());
    }
    auto type = assistx2::atoi_s(room->room_conifg_data()->type);
    stream.Write(type);
    stream.Write(room->room_conifg_data()->cost);
    
    stream.End();

    pImpl_->connector_->SendTo(stream.GetNativeStream());
}

LogServerImpl::LogServerImpl()
{
}

LogServerImpl::~LogServerImpl()
{
}

void LogServerImpl::WritePlayerRecord(uid_type mid, std::int32_t roomid, std::int32_t seatno, std::string uuid)
{
    assistx2::Stream stream(STANDARD_ROUTE_PACKET);
    stream.Write(static_cast<boost::int32_t>(SESSION_TYPE_LOGGER));
    stream.Write(0);
    stream.Write(XLOGGER_PLAYER_RECORD);
    stream.Write(static_cast<std::int32_t>(mid));
    stream.Write(roomid);
    stream.Write(seatno);
    stream.Write(uuid);
    stream.Write(game_type_);

    stream.End();

    connector_->SendTo(stream.GetNativeStream());
}

void LogServerImpl::WriteRoomRecord(std::int32_t roomid, std::string uuid)
{
    auto records = GameDataManager::getInstance()->GetRoomRecord(roomid);
    assistx2::Stream stream(STANDARD_ROUTE_PACKET);
    stream.Write(static_cast<boost::int32_t>(SESSION_TYPE_LOGGER));
    stream.Write(0);
    stream.Write(XLOGGER_ROOM_RECORD);
    stream.Write(roomid);
    stream.Write(uuid);
    stream.Write(records);
    stream.Write(game_type_);

    stream.End();

    connector_->SendTo(stream.GetNativeStream());
}

void LogServerImpl::StartOnlineLogTimer()
{
    WriteOnlineLog();
    GlobalTimerProxy::getInstance()->NewTimer(std::bind(&LogServerImpl::StartOnlineLogTimer,this),60);
}


void LogServerImpl::WriteOnlineLog()
{
    auto& login_players = GameDataManager::getInstance()->login_players();
    auto& playing_players = GameDataManager::getInstance()->playing_players();

    for (auto iter : login_players)
    {
        auto playingnum = 0;
        auto it = playing_players.find(iter.first);
        if (it != playing_players.end())
        {
            playingnum = it->second.size();
        }
        assistx2::Stream stream(STANDARD_ROUTE_PACKET);
        stream.Write(static_cast<boost::int32_t>(SESSION_TYPE_LOGGER));
        stream.Write(0);
        stream.Write(XLOGGER_PLAYER_ONLINE);
        stream.Write(static_cast<std::int32_t>(time(nullptr)));
        stream.Write(playingnum);
        stream.Write(iter.first);
        stream.Write(std::string(""));
        stream.Write(static_cast<std::int32_t>(iter.second.size()));
        stream.Write(game_type_); //gametype

        stream.End();

        connector_->SendTo(stream.GetNativeStream());
    }
}
