#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/configure.h>
#include "robotmanager.h"
#include "configmgr.h"
#include "robotagent.h"
#include "roombase.h"
#include "table.h"
#include <list>
#include <algorithm>

class RobotManagerImpl
{
public:
    RobotManagerImpl();
    ~RobotManagerImpl();

public:
    std::list< std::shared_ptr<Agent >> active_robots_;
    std::list<std::shared_ptr<Agent > > robot_pool_;//未分配的机器人
};

RobotManager::RobotManager():
 pImpl_(new RobotManagerImpl)
{

}

RobotManager::~RobotManager()
{
    pImpl_->active_robots_.clear();
    pImpl_->robot_pool_.clear();
}

bool RobotManager::Initialize(WatchDog* obj)
{
    // 初始化机器人池
    std::int32_t robot_begin = 0;
    std::int32_t robot_end = 0;
    auto cfg_reader = ConfigMgr::getInstance()->app_config_obj();
    cfg_reader->getConfig("robot", "begin", robot_begin);
    cfg_reader->getConfig("robot", "end", robot_end);
    for (int i = robot_begin; i <= robot_end; i++)
    {
        if (i == 0)
        {
            continue;
        }
        auto robot = std::make_shared<RobotAgent>();
        robot->set_uid(i);
        robot->set_watch_dog(obj);
        pImpl_->robot_pool_.push_back(robot);
    }

    return true;
}

std::shared_ptr<Agent > RobotManager::GetRobot()
{
    if (pImpl_->robot_pool_.empty())
    {
        return nullptr;
    }

    return pImpl_->robot_pool_.front();
}

void RobotManager::RecoverRobot(std::shared_ptr<Agent > robot)
{
    robot->Serialize(false);
    pImpl_->robot_pool_.push_back(robot);
    auto it = std::find_if(pImpl_->active_robots_.begin(), pImpl_->active_robots_.end(), 
        [robot](const std::shared_ptr<Agent > value) {
        return robot->uid() == value->uid();
    });
    if (it != pImpl_->active_robots_.end())
    {
        pImpl_->active_robots_.erase(it);
    }
}

void RobotManager::AttachRobot(RoomBase* room)
{
    if (room->room_state() == RoomBase::RoomState::CLOSED)
    {
        return;
    }
    auto seat_num = static_cast<std::int32_t>( 
        room->table_obj()->GetSeats().size());
    auto player_num = room->table_obj()->player_count();
    for (std::int32_t i = 0; i < (seat_num - player_num);++i)
    {
        auto robot = GetRobot();
        if (robot == nullptr)
        {
           continue;
        }
        if ( !robot->Serialize(true) )
        {
            continue;
        }
        robot->set_scene_object(room);
        auto res = room->Enter(robot);
        if (res <= 0)
        {
            robot->set_scene_object(nullptr);
            continue;
        }
        pImpl_->active_robots_.push_back(robot);
        pImpl_->robot_pool_.pop_front();
    }
}

RobotManagerImpl::RobotManagerImpl()
{
}

RobotManagerImpl::~RobotManagerImpl()
{
}