#ifndef FRAMEWORK_SCENE_GAMEHALL_H_
#define FRAMEWORK_SCENE_GAMEHALL_H_

#include "scenebase.h"
#include <memory>

class GameHallImpl;
class GameHall :public SceneBase
{
public:
    GameHall();
    virtual ~GameHall();
public:
    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);
private:
    std::unique_ptr< GameHallImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_GAMEHALL_H_