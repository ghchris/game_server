#ifndef FRAMEWORK_SCENE_SCENEBASE_H_
#define FRAMEWORK_SCENE_SCENEBASE_H_

#include "scene.h"
#include <memory>
#include "scenetimer.h"

class Seat;
class SceneTimer;
class SceneBaseImpl;
class SceneBase:public Scene
{
public:
    SceneBase(std::uint32_t id,std::string type);
    virtual ~SceneBase();

    const std::uint32_t player_count() const;
    const std::map<uid_type, std::shared_ptr<Agent > > & players_agent() const;
    void BroadCast(assistx2::Stream & packet, std::shared_ptr<Agent > exclude = nullptr,bool needsave = false);

    void set_scene_timer(std::shared_ptr<SceneTimer> obj);
    const std::shared_ptr<SceneTimer> scene_timer() const;
public:
    virtual const std::uint32_t scene_id() const;
    virtual const std::string scene_type() const;

    virtual void set_watchdog_obj(WatchDog* obj);
    virtual WatchDog* watchdog_obj();

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    virtual std::int32_t Enter(std::shared_ptr<Agent > player);
    virtual std::int32_t Leave(std::shared_ptr<Agent > player);
protected:
    void NewTimer(long expires_from_now, SceneTimerContext::TimerType type, Seat * seat = nullptr);
    bool CancelTimer(SceneTimerContext::TimerType type, Seat * seat = nullptr);

    virtual void OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context);
private:
    std::unique_ptr< SceneBaseImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_SCENEBASE_H_