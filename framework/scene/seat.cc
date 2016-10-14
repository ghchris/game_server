#include "seat.h"

class SeatImpl
{
public:
    SeatImpl();
    ~SeatImpl();
public:
    std::int32_t seat_no_;
    std::shared_ptr<Agent > player_;
    Seat::SeatState seat_state_ = Seat::SeatState::EMPTY;
    std::uint32_t seat_player_state_ = 0;
    Data data_;
};

Seat::Seat(std::int32_t no):
 pImpl_(new SeatImpl)
{
    pImpl_->seat_no_ = no;
}

Seat::~Seat()
{

}

void Seat::set_player(std::shared_ptr<Agent > player)
{
    pImpl_->player_ = player;
}

const std::shared_ptr<Agent > Seat::player() const
{
    return pImpl_->player_;
}

const std::int32_t Seat::seat_no() const
{
    return pImpl_->seat_no_;
}

void Seat::set_seat_state(const SeatState& state)
{
    pImpl_->seat_state_ = state;
}

const Seat::SeatState& Seat::seat_state() const
{
    return pImpl_->seat_state_;
}

void Seat::set_seat_player_state(const std::uint32_t state)
{
    pImpl_->seat_player_state_ = state;
}

const std::uint32_t Seat::seat_player_state()
{
    return pImpl_->seat_player_state_;
}

Data* Seat::data()
{
    return &pImpl_->data_;
}

SeatImpl::SeatImpl()
{
}

SeatImpl::~SeatImpl()
{
}