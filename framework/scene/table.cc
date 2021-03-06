#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "table.h"
#include <vector>
#include "seat.h"
#include "agent.h"

class TableImpl
{
public:
    TableImpl();
    ~TableImpl();
public:
    std::vector<Seat * > seats_;
    std::int32_t player_count_ = 0;
    std::int32_t max_seat_ = 0;
};

Table::Table(std::int32_t seat_count):
 pImpl_(new TableImpl)
{
    pImpl_->max_seat_ = seat_count;
    for (std::int32_t  i = BEGIN_SEAT; i <= seat_count; ++i)
    {
        pImpl_->seats_.push_back(new Seat(i));
    }
}

Table::~Table()
{
    for (auto iter : pImpl_->seats_)
    {
        delete iter;
    }

    pImpl_->seats_.clear();
}

std::int32_t Table::Enter(std::shared_ptr<Agent> player, const std::int32_t seatno)
{
    DCHECK(player != nullptr);

    auto seat = INVALID_SEAT;
    if (seatno == INVALID_SEAT)
    {
        for (auto iter : pImpl_->seats_)
        {
            if (iter->seat_state() == Seat::SeatState::EMPTY)
            {
                seat = iter->seat_no();
                iter->set_player( player );
                iter->set_seat_state( Seat::SeatState::USING );
                auto state = ((iter->seat_player_state() & Seat::PLAYER_STATUS_NET_CLOSE) | Seat::PLAYER_STATUS_WAITING);
                iter->set_seat_player_state(state);
                player->set_seat_no(seat);
                break;
            }
        }
        if (seat == INVALID_SEAT)
        {
            return -1;
        }
    }
    else
    {
        auto seat_obj = GetBySeatNo(seatno);
        if (seat_obj->seat_state() == Seat::SeatState::EMPTY)
        {
            seat = seatno;
            seat_obj->set_player(player);
            seat_obj->set_seat_state(Seat::SeatState::USING);
            auto state = ((seat_obj->seat_player_state() & Seat::PLAYER_STATUS_NET_CLOSE) | Seat::PLAYER_STATUS_WAITING);
            seat_obj->set_seat_player_state(state);
            player->set_seat_no(seatno);
        }
        else
        {
            return -2;
        }
    }

    pImpl_->player_count_ += 1;

    DLOG(INFO) << "Enter mid:=" << player->uid() << ", playercount_:=" << pImpl_->player_count_;
    DCHECK(pImpl_->player_count_ >= BEGIN_SEAT && pImpl_->player_count_ <= pImpl_->max_seat_);

    return seat;
}

std::int32_t Table::Leave(std::shared_ptr<Agent> player)
{
    auto seat = GetByUid(player->uid());
    DCHECK(seat != nullptr && seat->player() != nullptr);

    seat->set_player( nullptr );
    seat->set_seat_state( Seat::SeatState::EMPTY );
    seat->set_seat_player_state( Seat::PLAYER_STATUS_WAITING );
    player->set_seat_no(INVALID_SEAT);

    pImpl_->player_count_ -= 1;

    DLOG(INFO) << "Leave mid:=" << player->uid() << ", playercount_:=" << pImpl_->player_count_;
    DCHECK(pImpl_->player_count_ >= 0 && pImpl_->player_count_ < pImpl_->max_seat_);

    return 0;
}

Seat* Table::GetByUid(uid_type uid)
{
    for (auto iter : pImpl_->seats_)
    {
        if (iter->seat_state() == Seat::SeatState::USING &&
            iter->player()->uid() == uid)
        {
            return iter;
        }
    }

    return nullptr;
}

Seat* Table::GetBySeatNo(std::int32_t seatno)
{
    DCHECK(seatno >= BEGIN_SEAT && seatno <= pImpl_->max_seat_);
    return pImpl_->seats_.at(seatno - 1);
}

const std::int32_t Table::player_count() const
{
    return pImpl_->player_count_;
}

const std::vector< Seat* >& Table::GetSeats() const
{
    return pImpl_->seats_;
}

Seat* Table::prev_seat(std::int32_t seatno)
{
    if (seatno > 1)
    {
        return GetBySeatNo(seatno - 1);
    }
    else
    {
        return GetBySeatNo(pImpl_->max_seat_);
    }
}

Seat* Table::next_seat(std::int32_t seatno)
{
    if (seatno < pImpl_->max_seat_)
    {
        return GetBySeatNo(seatno + 1);
    }
    else
    {
        return GetBySeatNo(1);
    }
}

TableImpl::TableImpl()
{
}

TableImpl::~TableImpl()
{
}