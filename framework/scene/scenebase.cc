#include "scenebase.h"
#include "agent.h"

class SceneBaseImpl
{
public:
    SceneBaseImpl();
    ~SceneBaseImpl();
public:
    std::map<uid_type, std::shared_ptr<Agent > > players_agent_;
    std::uint32_t scene_id_;
    std::string scene_type_;
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

void SceneBase::BroadCast(assistx2::Stream & packet)
{

}

const std::uint32_t SceneBase::scene_id() const
{
    return pImpl_->scene_id_;
}

const std::string SceneBase::scene_type() const
{
    return pImpl_->scene_type_;
}

const std::uint32_t SceneBase::player_count() const
{
    return pImpl_->players_agent_.size();
}

const std::map<uid_type, std::shared_ptr<Agent > > & SceneBase::players_agent() const
{
    return pImpl_->players_agent_;
}

SceneBaseImpl::SceneBaseImpl()
{
}

SceneBaseImpl::~SceneBaseImpl()
{
}