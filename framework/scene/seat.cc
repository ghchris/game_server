#include "seat.h"

class SeatImpl
{
public:
    SeatImpl();
    ~SeatImpl();
public:
    std::int32_t seat_no_;
    uid_type player_uid_;
    Seat::SeatState seat_state_ = Seat::SeatState::EMPTY;
    std::uint32_t seat_player_state_ = 0;
};

Seat::Seat(std::int32_t no):
 pImpl_(new SeatImpl)
{
    pImpl_->seat_no_ = no;
}

Seat::~Seat()
{

}

void Seat::set_player_uid(const uid_type uid)
{
    pImpl_->player_uid_ = uid;
}

const uid_type Seat::player_uid() const
{
    return pImpl_->player_uid_;
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

SeatImpl::SeatImpl()
{
}

SeatImpl::~SeatImpl()
{
}