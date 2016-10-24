#include "scenebase.h"
#include "agent.h"

class SceneBaseImpl
{
public:
    SceneBaseImpl();
    ~SceneBaseImpl();
public:
    std::map<uid_type, std::shared_ptr<Agent > > players_agent_;
    std::uint32_t scene_id_ = 0;
    std::string scene_type_ = "";
    std::shared_ptr<SceneTimer> scene_timer_ = nullptr;
};

SceneBase::SceneBase(std::uint32_t id, std::string type):
 pImpl_(new SceneBaseImpl)
{
    pImpl_->scene_id_ = id;
    pImpl_->scene_type_ = type;
}

SceneBase::~SceneBase()
{

}

std::int32_t SceneBase::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    return 0;
}

void SceneBase::OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context)
{

}

std::int32_t SceneBase::Enter(std::shared_ptr<Agent > player)
{
    auto uid = player->uid();
    auto iter = pImpl_->players_agent_.find(uid);
    if (iter == pImpl_->players_agent_.end())
    {
        pImpl_->players_agent_.insert(std::make_pair(uid, player));
    }

    return 0;
}

std::int32_t SceneBase::Leave(std::shared_ptr<Agent > player)
{
    auto uid = player->uid();
    auto iter = pImpl_->players_agent_.find(uid);
    if (iter != pImpl_->players_agent_.end())
    {
        pImpl_->players_agent_.erase(iter);
    }

    return 0;
}

void SceneBase::BroadCast(assistx2::Stream & packet,std::shared_ptr<Agent > exclude)
{
    for (auto iter : pImpl_->players_agent_)
    {
        if (iter.second->connect_status() == false)
        {
            continue;
        }
        if (exclude == nullptr || exclude->uid() != iter.second->uid())
        {
            iter.second->SendTo(packet);
        }
    }
}

const std::uint32_t SceneBase::scene_id() const
{
    return pImpl_->scene_id_;
}

const std::string SceneBase::scene_type() const
{
    return pImpl_->scene_type_;
}

void SceneBase::set_scene_timer(std::shared_ptr<SceneTimer> obj)
{
    pImpl_->scene_timer_ = obj;
}

const std::shared_ptr<SceneTimer> SceneBase::scene_timer() const
{
    return pImpl_->scene_timer_;
}

const std::uint32_t SceneBase::player_count() const
{
    return pImpl_->players_agent_.size();
}

const std::map<uid_type, std::shared_ptr<Agent > > & SceneBase::players_agent() const
{
    return pImpl_->players_agent_;
}

void SceneBase::NewTimer(long expires_from_now, SceneTimerContext::TimerType type, Seat * seat)
{
    const auto err = pImpl_->scene_timer_->NewTimer(
        expires_from_now, boost::bind(&SceneBase::OnTimer, this, _1), scene_id(), type, seat);
    if (err != 0)
    {
        EventTimerContext id(boost::bind(&SceneBase::OnTimer, this, _1), scene_id(), type, seat);
        pImpl_->scene_timer_->CancelTimer(id);

        pImpl_->scene_timer_->NewTimer(expires_from_now, boost::bind(&SceneBase::OnTimer, this, _1),
            scene_id(), type, seat);
    }
}

bool SceneBase::CancelTimer(SceneTimerContext::TimerType type, Seat * seat)
{
    EventTimerContext id(boost::bind(&SceneBase::OnTimer, this, _1), scene_id(), type, seat);
    return pImpl_->scene_timer_->CancelTimer(id);
}

SceneBaseImpl::SceneBaseImpl()
{
}

SceneBaseImpl::~SceneBaseImpl()
{
}