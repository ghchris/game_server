#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "gamedatamanager.h"
#include <json_spirit_writer_template.h>
#include <assistx2/json_wrapper.h>
#include "privateroom.h"
#include "table.h"
#include "seat.h"
#include "agent.h"
#include "seatdata.h"
#include "cardgroup.h"
#include "common.h"
#include "memberfides.pb.h"
#include "datalayer.h"
#include "watchdog.h"
#include "scenemanager.h"

class GameDataManagerImpl
{
public:
    struct SeatPlayerData
    {
        std::int32_t mid = 0;
        std::int32_t seatno = 0;
        std::string data;
    };
public:
    GameDataManagerImpl();
    ~GameDataManagerImpl();
    template< class T>
    json_spirit::Array & WriteRoomRecord(const int room, const T & t);
    void OnUpdatePlayingPlayers(std::int32_t gp, uid_type mid,bool isAdd = true);
    std::vector<SeatPlayerData> ParseRoomSeatsData(std::string data);
    void ResetRoomPlayerData(RoomBase* room,const std::vector<SeatPlayerData>& playerdata);
    void UpdataPlayedNum(RoomBase* room,std::string data);
public:
    std::map<int, json_spirit::Array > room_records_;
    std::map<std::int32_t, std::set<uid_type> > login_players_;
    std::map<std::int32_t, std::set<uid_type> > playing_players_;
    WatchDog* watch_dog_;
    std::map<uid_type,std::vector< std::pair<std::int16_t, assistx2::Stream> >> vc_save_stream_;
    std::vector<std::uint32_t> actived_rooms_;
};

GameDataManager::GameDataManager():
 pImpl_(new GameDataManagerImpl)
{

}

GameDataManager::~GameDataManager()
{

}

void GameDataManager::Initialize(WatchDog* wtachdog)
{
    pImpl_->watch_dog_ = wtachdog;
}

void GameDataManager::OnGameStart(RoomBase* room)
{
    json_spirit::Array array;
    array.push_back(DATA_GAME_START);
    auto value = Common::StringToJson(room->RoomDataToString(false));
    array.push_back(value);
    auto& seats = room->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->player() == nullptr)
        {
            continue;
        }
        json_spirit::Object seat_json;
        seat_json.push_back(json_spirit::Pair("mid", static_cast<std::int32_t>(iter->player()->uid())));
        seat_json.push_back(json_spirit::Pair("seatno", iter->seat_no()));
        seat_json.push_back(json_spirit::Pair("score", iter->data()->seat_score_));
        std::string strhandcards;
        auto& hand_cards = iter->data()->hand_cards_->hand_cards();
        for (auto it : hand_cards)
        {
            strhandcards += it->getName() + ",";
        }
        seat_json.push_back(json_spirit::Pair("cards", strhandcards));
        array.push_back(seat_json);
        pImpl_->OnUpdatePlayingPlayers(iter->player()->member_fides()->gp(), iter->player()->uid());
        if (iter->player()->agent_type() == Agent::AgentType::PLAYER)
        {
            DataLayer::getInstance()->set_playing_player_to_cache(iter->player()->uid(), 
                pImpl_->watch_dog_->game_session());
        }
    }

    pImpl_->WriteRoomRecord(room->scene_id(), array);
}

void GameDataManager::OnOperation(RoomBase* room, std::int32_t seatno, 
    CardLogic::OperationType operation, std::shared_ptr< Card >card, MingTangType type)
{
    json_spirit::Array array;
    array.push_back(DATA_PLAY);
    array.push_back(seatno);
    array.push_back(static_cast<std::int32_t>(operation));
    if (card == nullptr)
    {
        array.push_back(std::string(""));
    }
    else
    {
        array.push_back(card->getName());
    }
    array.push_back(static_cast<std::int32_t>(type));
    pImpl_->WriteRoomRecord(room->scene_id(), array);
}

void GameDataManager::OnGameOver(RoomBase* room)
{
    json_spirit::Array array;
    array.push_back(DATA_GAME_OVER);
    auto& seats = room->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->player() == nullptr)
        {
            continue;
        }
        json_spirit::Object seat_json;
        seat_json.push_back(json_spirit::Pair("mid", static_cast<std::int32_t>(iter->player()->uid())));
        seat_json.push_back(json_spirit::Pair("seatno", iter->seat_no()));
        seat_json.push_back(json_spirit::Pair("score", iter->data()->seat_score_));
        array.push_back(seat_json);
        pImpl_->OnUpdatePlayingPlayers(iter->player()->member_fides()->gp(), iter->player()->uid(),false);
        if (iter->player()->agent_type() == Agent::AgentType::PLAYER)
        {
            DataLayer::getInstance()->remove_playing_player_from_cache(iter->player()->uid());
        }
    }

    pImpl_->WriteRoomRecord(room->scene_id(), array);
}

void GameDataManager::OnZhaMa(RoomBase* room, const Cards& card)
{
    json_spirit::Array array;
    array.push_back(DATA_ZHAMA);
    for (auto iter : card)
    {
        array.push_back(iter->getName());
    }

    pImpl_->WriteRoomRecord(room->scene_id(), array);
}

std::string GameDataManager::GetRoomRecord(std::int32_t roomid)
{
    std::string records = "";
    auto iter = pImpl_->room_records_.find(roomid);
    if (iter != pImpl_->room_records_.end())
    {
        records = json_spirit::write_string(json_spirit::Value(iter->second));
        pImpl_->room_records_.erase(iter);
    }

    return records;
}

void GameDataManager::OnLogin(std::int32_t gp, uid_type mid)
{
    auto iter = pImpl_->login_players_.find(gp);
    if (iter != pImpl_->login_players_.end())
    {
        iter->second.insert(mid);
    }
    else
    {
        std::set<uid_type > tmp;
        tmp.insert(mid);
        pImpl_->login_players_.insert(make_pair(gp, tmp));
    }
}

void GameDataManager::OnLogout(std::int32_t gp, uid_type mid)
{
    auto iter = pImpl_->login_players_.find(gp);
    if (iter != pImpl_->login_players_.end())
    {
        iter->second.erase(mid);
    }
}

void GameDataManager::ResetRoomData(RoomBase* room)
{
    std::string data;
    auto res = DataLayer::getInstance()->room_data_from_cache(
        room->room_index(), data);
    if (res == false)
    {
        return;
    }

    pImpl_->actived_rooms_.push_back(room->scene_id());

    room->set_room_state(RoomBase::RoomState::WAITING);
    SceneManager::getInstance()->AttachActivedPrivateRoom(room);

    room->StringToRoomData(data);

    auto vcPlayerData = pImpl_->ParseRoomSeatsData(data);

    pImpl_->ResetRoomPlayerData(room, vcPlayerData);
}

bool GameDataManager::UpdataRoomData(RoomBase* room)
{
    auto iter = std::find_if(pImpl_->actived_rooms_.begin(), pImpl_->actived_rooms_.end(), 
        [room](const std::uint32_t value) {
        return room->scene_id() == value;
    });
    if (iter == pImpl_->actived_rooms_.end())
    {
        return true;
    }

    std::string data;
    auto res = DataLayer::getInstance()->room_data_from_cache(
        room->room_index(), data);
    if (res == false)
    {
        room->Disband();
        pImpl_->actived_rooms_.erase(iter);
        return false;
    }

    pImpl_->UpdataPlayedNum(room, data);

    auto vcPlayerData = pImpl_->ParseRoomSeatsData(data);

    for (auto iter : vcPlayerData)
    {
        if (iter.mid <= 0)
        {
            continue;
        }
        auto seat = room->table_obj()->GetBySeatNo(iter.seatno);
        DCHECK(seat != nullptr);
        seat->data()->set_string_to_seat_data(iter.data);
    }
    pImpl_->actived_rooms_.erase(iter);

    return true;
}

void GameDataManager::SaveCmdStream(uid_type mid,const assistx2::Stream& stream)
{
    assistx2::Stream clone(stream);
    const auto cmd = clone.GetCmd();

    auto iter = pImpl_->vc_save_stream_.find(mid);
    if (iter == pImpl_->vc_save_stream_.end())
    {
        pImpl_->vc_save_stream_.emplace(mid,
            std::vector<std::pair<std::int16_t, assistx2::Stream>>(1,std::make_pair(cmd,clone)));
    }
    else
    {
        auto it = std::find_if(iter->second.begin(), iter->second.end(),
            [cmd](const std::pair<std::int16_t, assistx2::Stream>& value) {
            return cmd == value.first;
        });
        if (it != iter->second.end())
        {
            it->second = clone;
        }
        else
        {
            iter->second.push_back(std::make_pair(cmd,clone));
        }
    }
}

void GameDataManager::DeleteCmdStream(uid_type mid, const std::int16_t cmd)
{
    auto iter = pImpl_->vc_save_stream_.find(mid);
    if (iter != pImpl_->vc_save_stream_.end())
    {
        auto it = std::find_if(iter->second.begin(), iter->second.end(),
            [cmd](const std::pair<std::int16_t, assistx2::Stream>& value) {
            return cmd == value.first;
        });
        if (it != iter->second.end())
        {
            iter->second.erase(it);
        }
        if (iter->second.size() == 0)
        {
            pImpl_->vc_save_stream_.erase(iter);
        }
    }
}

const std::vector<std::pair<std::int16_t, assistx2::Stream>> GameDataManager::GetCmdStream(uid_type mid) const
{
    auto iter = pImpl_->vc_save_stream_.find(mid);

    if (iter != pImpl_->vc_save_stream_.end())
    {
        return iter->second;
    }

    return std::vector<std::pair<std::int16_t, assistx2::Stream>>();
}

const std::map<std::int32_t, std::set<uid_type> >& GameDataManager::login_players() const
{
    return pImpl_->login_players_;
}

const std::map<std::int32_t, std::set<uid_type> >& GameDataManager::playing_players() const
{
    return pImpl_->playing_players_;
}

GameDataManagerImpl::GameDataManagerImpl()
{
}

GameDataManagerImpl::~GameDataManagerImpl()
{
}

template< class T>
json_spirit::Array & GameDataManagerImpl::WriteRoomRecord(const int room, const T & t)
{
    std::map<int, json_spirit::Array >::iterator it = room_records_.find(room);
    if (it != room_records_.end())
    {
        it->second.push_back(t);
    }
    else
    {
        json_spirit::Array tmp;
        tmp.push_back(t);
        it = room_records_.insert(make_pair(room, tmp)).first;
    }

    return it->second;
}

void GameDataManagerImpl::OnUpdatePlayingPlayers(std::int32_t gp, uid_type mid, bool isAdd)
{
    auto iter = playing_players_.find(gp);
    if (iter != playing_players_.end())
    {
        if (isAdd == true)
        {
            iter->second.insert(mid);
        }
        else
        {
            iter->second.erase(mid);
        }
    }
    else
    {
        if (isAdd == true)
        {
            std::set<uid_type > tmp;
            tmp.insert(mid);
            playing_players_.insert(make_pair(gp, tmp));
        }
    }
}

std::vector<GameDataManagerImpl::SeatPlayerData> 
GameDataManagerImpl::ParseRoomSeatsData(std::string data)
{
    std::vector<SeatPlayerData> vcPlayerData;

    auto value = Common::StringToJson(data);
    auto & obj = value.get_obj();
    json_spirit::Array seats;
    for (auto iter : obj)
    {
        if (iter.name_ == "seats")
        {
            seats = iter.value_.get_array();
            break;
        }
    }
    for (auto iter : seats)
    {
        auto seat = iter.get_obj();
        SeatPlayerData player_data;
        for (auto it : seat)
        {
            if (it.name_ == "mid")
            {
                player_data.mid = assistx2::ToInt32(it.value_);
                continue;
            }
            if (it.name_ == "seatno")
            {
                player_data.seatno = assistx2::ToInt32(it.value_);
                continue;
            }
            if (it.name_ == "data")
            {
                player_data.data = assistx2::ToString(it.value_);
                continue;
            }
        }
        vcPlayerData.push_back(player_data);
    }

    return vcPlayerData;
}

void GameDataManagerImpl::ResetRoomPlayerData(RoomBase* room, const std::vector<SeatPlayerData>& playerdata)
{
    for (auto iter : playerdata)
    {
        if (iter.mid <= 0)
        {
            continue;
        }
        auto player = watch_dog_->NewAgent(iter.mid);
        DCHECK(player != nullptr);
        player->set_seat_no(iter.seatno);
        auto seat = room->table_obj()->GetBySeatNo(iter.seatno);
        DCHECK(seat != nullptr);
        seat->data()->set_string_to_seat_data(iter.data);
        auto state = seat->seat_player_state() | Seat::PLAYER_STATUS_NET_CLOSE;
        seat->set_seat_player_state(state);
        CHECK(room->Enter(player) > 0);
        player->set_scene_object(room);
    }
}

void  GameDataManagerImpl::UpdataPlayedNum(RoomBase* room, std::string data)
{
    auto value = Common::StringToJson(data);
    auto & obj = value.get_obj();
    json_spirit::Array seats;
    for (auto iter : obj)
    {
        if (iter.name_ == "playednum")
        {
            auto privateroom = dynamic_cast<PrivateRoom*>(room);
            privateroom->set_played_num(assistx2::ToInt32(iter.value_));
            break;
        }
    }
}
