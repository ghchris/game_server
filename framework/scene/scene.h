#ifndef FRAMEWORK_SCENE_SCENE_H_
#define FRAMEWORK_SCENE_SCENE_H_

#include <map>
#include <memory>
#include "define.h"

namespace assistx2 {
    class Stream;
    namespace timer2 {
        class TimerContext;
    }
}

class Agent;
class Scene
{
public:
    virtual const std::uint32_t scene_id() const = 0;
    virtual const std::string scene_type() const = 0;

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > /*user*/, assistx2::Stream * /*packet*/) = 0;
    virtual std::int32_t Enter(std::shared_ptr<Agent > player) = 0;
    virtual std::int32_t Leave(std::shared_ptr<Agent > player) = 0;
};

#endif //FRAMEWORK_SCENE_SCENE_H_