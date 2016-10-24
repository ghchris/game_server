#ifndef FRAMEWORK_GAME_ROBOTMANAGER_H_
#define FRAMEWORK_GAME_ROBOTMANAGER_H_

#include <memory>
#include <assistx2/singleton.h>

class WatchDog;
class Agent;
class RoomBase;
class RobotManagerImpl;
class RobotManager :public Singleton<RobotManager >
{
public:
    RobotManager();
    virtual ~RobotManager();

    bool Initialize(WatchDog* obj);
    std::shared_ptr<Agent > GetRobot();
    void RecoverRobot(std::shared_ptr<Agent > robot);

    void AttachRobot(RoomBase* room);
private:
    std::unique_ptr< RobotManagerImpl > pImpl_;
};

#endif
