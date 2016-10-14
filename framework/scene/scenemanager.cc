#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "scenemanager.h"
#include "GameHall.h"
#include "gameconfigdata.h"
#include "hzmajiangroom.h"

class SceneManagerImpl
{
public:
    SceneManagerImpl();
    ~SceneManagerImpl();
public:
    GameHall* game_hall_;
    std::map<std::int32_t, RoomBase * > rooms_;
    std::map<std::int32_t, RoomBase * > actived_private_rooms_;
    std::map<std::string, std::vector<RoomBase * > > room_groups_;
};

SceneManager::SceneManager():
 pImpl_(new SceneManagerImpl)
{

}

SceneManager::~SceneManager()
{
    if (pImpl_->game_hall_ != nullptr)
    {
        delete pImpl_->game_hall_;
        pImpl_->game_hall_ = nullptr;
    }

    for (auto iter : pImpl_->rooms_)
    {
        delete iter.second;
    }

    pImpl_->rooms_.clear();
}

bool SceneManager::Initialize()
{
    pImpl_->game_hall_ = new GameHall;

    GameConfigData::getInstance()->Init(); 

    auto& rooms_data = GameConfigData::getInstance()->rooms_data();
    for (auto iter : rooms_data)
    {
        RoomBase* room = nullptr;
        for (std::int32_t i = iter->begin; i <= iter->end; ++i)
        {
            if (iter->type == "1")
            {
                room = new HzMajiangRoom(iter->player,i,iter->type);
                room->set_room_config_data(iter);
            }
            else
            {
                continue;
            }

            pImpl_->rooms_.insert(std::make_pair(i, room));

            auto it = pImpl_->room_groups_.find(iter->type);
            if (it != pImpl_->room_groups_.end())
            {
                it->second.push_back(room);
            }
            else
            {
                pImpl_->room_groups_.insert(std::make_pair(iter->type,
                    std::vector<RoomBase* >(1, room)));
            }
        }
    }
    return true;
}

Scene* SceneManager::default_scene()
{
    return pImpl_->game_hall_;
}

RoomBase* SceneManager::GetRoomByType(const std::string& type, std::int32_t ju)
{
    DLOG(ERROR) << "GetRoomByType type:=" << type << " ju:=" << ju;

    auto iter = pImpl_->room_groups_.find(type);
    if (iter == pImpl_->room_groups_.end())
    {
        return nullptr;
    }

    for (auto room : iter->second)
    {
        if (room->room_state() != RoomBase::RoomState::CLOSED)
        {
            continue;
        }
        if (room->room_conifg_data()->ju != ju)
        {
            continue;
        }
        return room;
    }

    return nullptr;
}

void SceneManager::AttachActivedPrivateRoom(RoomBase* room)
{
    auto it = pImpl_->actived_private_rooms_.find(room->scene_id());
    if (it == pImpl_->actived_private_rooms_.end())
    {
        pImpl_->actived_private_rooms_.insert(std::make_pair(
            room->scene_id(),room));
    }
    else
    {
        DLOG(ERROR) << "AttachActivedPrivateRoom: room:=" << room->scene_id()
            << "already in actived_private_rooms!";
    }
}

void SceneManager::DetachActivedPrivateRoom(RoomBase* room)
{
    auto it = pImpl_->actived_private_rooms_.find(room->scene_id());
    if (it != pImpl_->actived_private_rooms_.end())
    {
        pImpl_->actived_private_rooms_.erase(it);
    }
    else
    {
        DLOG(ERROR) << "DetachActivedPrivateRoom: room:=" << room->scene_id()
            << "was not found in actived_private_rooms";
    }
}

RoomBase* SceneManager::GetRoomFromActivedPrivateRoom(const std::int32_t roomid)
{
    auto it = pImpl_->actived_private_rooms_.find(roomid);
    if (it != pImpl_->actived_private_rooms_.end())
    {
        return it->second;
    }
    
    return nullptr;
}

SceneManagerImpl::SceneManagerImpl()
{
}

SceneManagerImpl::~SceneManagerImpl()
{
}