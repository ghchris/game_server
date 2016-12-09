#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "agentbase.h"
#include "scene.h"

class AgentBaseImpl
{
public:
    AgentBaseImpl();
    ~AgentBaseImpl();
public:
    uid_type uid_ = 0;
    Agent::AgentType agent_type_ = Agent::AgentType::PLAYER;
    Scene* scene_obj_ = nullptr;
    std::string ip_addr_;
    std::int32_t game_session_ = 0;
    WatchDog* wathc_dog_ = nullptr;
    bool connect_status_ = false;
    std::int32_t seat_no_ = 0;
};

AgentBase::AgentBase(const AgentType type):
    pImpl_(new AgentBaseImpl)
{
    pImpl_->agent_type_ = type;
}

AgentBase::~AgentBase()
{
    DLOG(INFO) << "mid:=" << uid()
        << " connect_status:=" << connect_status()
        << " Has been Destroyed!";
}

bool AgentBase::Serialize(bool loadorsave)
{
    return true;
}

void AgentBase::Process(assistx2::Stream * packet)
{
    if (pImpl_->scene_obj_ != nullptr)
    {
        pImpl_->scene_obj_->
            OnMessage(shared_from_this(),packet);
    }
    else
    {
        LOG(ERROR) << "AgentBase::Process: mid:="
            << uid() << " Now Not In Any Scene!";
    }
}

void AgentBase::SendTo(const assistx2::Stream& packet, bool needsave)
{
    //do nothing
}

bool AgentBase::GoldPay(const std::int64_t gold,
    const std::int32_t pay_type)
{
    return true;
}

const Agent::AgentType& AgentBase::agent_type() const
{
    return pImpl_->agent_type_;
}

void AgentBase::set_uid(const uid_type uid)
{
    pImpl_->uid_ = uid;
}

const uid_type AgentBase::uid() const
{
    return pImpl_->uid_;
}

void AgentBase::set_scene_object(Scene* obj)
{
    pImpl_->scene_obj_ = obj;
}

Scene* AgentBase::scene_object()
{
    return pImpl_->scene_obj_;
}

void AgentBase::set_ip_addr(const std::string& addr)
{
    pImpl_-> ip_addr_ = addr;
}

const std::string& AgentBase::ip_addr() const
{
    return pImpl_->ip_addr_;
}

void AgentBase::set_game_session(const std::int32_t session)
{
    pImpl_->game_session_ = session;
}

const std::int32_t AgentBase::game_session() const
{
    return pImpl_->game_session_;
}

void AgentBase::set_watch_dog(WatchDog * obj)
{
    pImpl_->wathc_dog_ = obj;
}

WatchDog* AgentBase::watch_dog()
{
    return pImpl_->wathc_dog_;
}

void AgentBase::set_connect_status(bool status)
{
    pImpl_->connect_status_ = status;
}

const bool AgentBase::connect_status() const
{
    return pImpl_->connect_status_;
}

void AgentBase::set_seat_no(const std::int32_t seatno)
{
    pImpl_->seat_no_ = seatno;
}

const std::int32_t AgentBase::seat_no() const
{
    return pImpl_->seat_no_;
}

AgentBaseImpl::AgentBaseImpl()
{
}

AgentBaseImpl::~AgentBaseImpl()
{
}