#ifndef FRAMEWORK_SRC_SCENEBASE_H_
#define FRAMEWORK_SRC_SCENEBASE_H_

#include "scene.h"
#include <memory>

class SceneBaseImpl;
class SceneBase:public Scene
{
public:
    SceneBase(std::uint32_t id,std::string type);
    virtual ~SceneBase();

    const std::uint32_t player_count() const;
    const std::map<uid_type, std::shared_ptr<Agent > > & players_agent() const;
    void BroadCast(assistx2::Stream & packet);
public:
    virtual const std::uint32_t scene_id() const;
    virtual const std::string scene_type() const;

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    virtual std::int32_t Enter(std::shared_ptr<Agent > player);
    virtual std::int32_t Leave(std::shared_ptr<Agent > player);
private:
    std::unique_ptr< SceneBaseImpl > pImpl_;
};

#endif