#ifndef FRAMEWORK_SRC_AGENTBASE_H_
#define FRAMEWORK_SRC_AGENTBASE_H_

#include "agent.h"
#include <memory>


class AgentBaseImpl;
class AgentBase:public Agent
{
public:
    explicit AgentBase(const AgentType type);
    virtual ~AgentBase();
public:
    virtual bool Serialize(bool loadorsave) ;

    virtual void Process(assistx2::Stream * packet);

    virtual void SendTo(const assistx2::Stream& packet);

    virtual bool GoldPay(const std::int64_t gold,
        const std::int32_t pay_type);

    virtual const AgentType& agent_type() const;

    virtual void set_uid(const uid_type uid);
    virtual const uid_type uid() const;

    virtual void set_scene_object(Scene* obj);
    virtual Scene* scene_object();

    virtual void set_ip_addr(const std::string& addr);
    virtual const std::string& ip_addr() const;

    virtual void set_game_session(const std::int32_t session);
    virtual const std::int32_t game_session() const;

    virtual void set_watch_dog(WatchDog * obj);
    virtual WatchDog* watch_dog();

    virtual void set_connect_status(bool status);
    virtual const bool connect_status() const;

    virtual void set_seat_no(const std::int32_t seatno);
    virtual const std::int32_t seat_no() const;
private:
    DISALLOW_COPY_AND_ASSIGN(AgentBase);
private:
    std::unique_ptr< AgentBaseImpl > pImpl_;
};

#endif //FRAMEWORK_SRC_AGENTBASE_H_
