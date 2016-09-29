#include "roombase.h"
#include "table.h"

class RoomBaseImpl
{
public:
    RoomBaseImpl();
    ~RoomBaseImpl();
public:
    RoomBase::RoomState room_state_ = RoomBase::RoomState::CLOSED;
    Table* table_ = nullptr;
};

RoomBase::RoomBase(std::int32_t size,std::uint32_t id, std::string type):
 SceneBase(id,type),
 pImpl_(new RoomBaseImpl)
{
    pImpl_->table_ = new Table(size);
}

RoomBase::~RoomBase()
{
    if (pImpl_->table_ != nullptr)
    {
        delete pImpl_->table_;
        pImpl_->table_ = nullptr;
    }
}

std::int32_t RoomBase::Enter(std::shared_ptr<Agent > player)
{
    auto res  = pImpl_->table_->Enter(player);
    if (res <= 0)
    {
        return res;
    }

    return SceneBase::Enter(player);
}

std::int32_t RoomBase::Leave(std::shared_ptr<Agent > player)
{
    pImpl_->table_->Leave(player);

    return SceneBase::Leave(player);
}

void RoomBase::set_room_state(const RoomState& state)
{
    pImpl_->room_state_ = state;
}

const RoomBase::RoomState& RoomBase::room_state() const
{
    return pImpl_->room_state_;
}

RoomBaseImpl::RoomBaseImpl()
{
}

RoomBaseImpl::~RoomBaseImpl()
{
}