#ifndef FRAMEWORK_SCENE_ROOM_H_
#define FRAMEWORK_SCENE_ROOM_H_

#include "scenebase.h"
#include <memory>

class RoomBaseImpl;
class RoomBase :public SceneBase
{
public:
    enum class RoomState {CLOSED,WAITING,PLAYING,FREEZING};
public:
    RoomBase(std::int32_t size,std::uint32_t id, std::string type);
    virtual ~RoomBase();

    virtual std::int32_t Enter(std::shared_ptr<Agent > player);
    virtual std::int32_t Leave(std::shared_ptr<Agent > player);

    void set_room_state(const RoomState& state);
    const RoomState& room_state() const;
private:
    std::unique_ptr< RoomBaseImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_ROOM_H_