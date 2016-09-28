/*=====================
    版权(copyright statement): 
    作者(author line): caocheng
======================*/
#ifndef FRAMEWORK_SRC_AGENT_H_
#define FRAMEWORK_SRC_AGENT_H_

#include "define.h"
#include <string>
#include <memory>
//agent 接口类,一个agent对应一个客户端

namespace assistx2 {
    class Stream;
}

class WatchDog;
class Scene;
class Agent: public std::enable_shared_from_this<Agent>
{
public:
    enum class AgentType{ ROBOT , PLAYER};
public:
    virtual bool Serialize(bool /*loadorsave*/) = 0;

    //处理客户端消息
    virtual void Process(assistx2::Stream * packet) = 0;

    //发消息给客户端
    virtual void SendTo(const assistx2::Stream& packet) = 0;

    //获取agent类型
    virtual const AgentType& agent_type() const = 0;

    //存取当前userid
    virtual void set_uid(const uid_type uid) = 0;
    virtual const uid_type uid() const = 0;

    //存取当前scene对象
    virtual void set_scene_object(Scene* obj) = 0;
    virtual Scene* scene_object()= 0;

    //存取当前IP地址
    virtual void set_ip_addr(const std::string& addr) = 0;
    virtual const std::string& ip_addr() const = 0;

    //存取当前游戏会话ID
    virtual void set_game_session(const std::int32_t session) = 0;
    virtual const std::int32_t game_session() const = 0;

    //存取watchdog对象
    virtual void set_watch_dog(WatchDog * obj) = 0;
    virtual WatchDog* watch_dog() = 0;

    //存取连接状态
    virtual void set_connect_status(bool status) = 0;
    virtual const bool connect_status() const = 0;
};

#endif //FRAMEWORK_SRC_AGENT_H_
