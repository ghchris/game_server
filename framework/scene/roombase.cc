#include "roombase.h"
#include "table.h"
class RoomBaseImpl
{
public:
    RoomBaseImpl();
    ~RoomBaseImpl();
public:
    RoomBase::RoomState room_state_ = RoomBase::RoomState::CLOSED;
    std::shared_ptr<Table> table_ = nullptr;
    uid_type room_owner_ = 0;
    std::shared_ptr<RoomData> room_data_ = nullptr;
    time_t create_time_ = 0;
};

RoomBase::RoomBase(std::uint32_t id, std::string type) :
 SceneBase(id, type),
 pImpl_(new RoomBaseImpl)
{
    
}

RoomBase::~RoomBase()
{
}

std::int32_t RoomBase::Enter(std::shared_ptr<Agent > player)
{
    return SceneBase::Enter(player);
}

std::int32_t RoomBase::Leave(std::shared_ptr<Agent > player)
{
    return SceneBase::Leave(player);
}

std::int32_t RoomBase::Disband()
{
    return 0;
}

void RoomBase::set_room_state(const RoomState& state)
{
    pImpl_->room_state_ = state;
}

const RoomBase::RoomState& RoomBase::room_state() const
{
    return pImpl_->room_state_;
}

void RoomBase::set_table_obj(std::shared_ptr<Table> obj)
{
    pImpl_->table_ = obj;
}

std::shared_ptr<Table> RoomBase::table_obj()
{
    return pImpl_->table_;
}

void RoomBase::set_room_owner(uid_type mid)
{
    pImpl_->room_owner_ = mid;
}

const uid_type RoomBase::room_owner() const
{
    return pImpl_->room_owner_;
}

void RoomBase::set_room_config_data(std::shared_ptr<RoomData> data)
{
    pImpl_->room_data_ = data;
}

const std::shared_ptr<RoomData> RoomBase::room_conifg_data() const
{
    return pImpl_->room_data_;
}

void RoomBase::set_create_time(const time_t time)
{
    pImpl_->create_time_ = time;
}

const time_t RoomBase::create_time() const
{
    return pImpl_->create_time_;
}

std::string RoomBase::RoomDataToString(bool isContainTableData)
{
    return "";
}

void RoomBase::StringToRoomData(const std::string& str)
{

}

RoomBaseImpl::RoomBaseImpl()
{
}

RoomBaseImpl::~RoomBaseImpl()
{
}