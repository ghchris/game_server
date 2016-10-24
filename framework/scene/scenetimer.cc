#include "scenetimer.h"

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "seat.h"

SceneTimer::SceneTimer(boost::asio::io_service & ios) :ios_(ios), queue_(nullptr)
{

}

SceneTimer::~SceneTimer(void)
{
    if (queue_ != nullptr)
    {
        queue_->Destroy();
        queue_ = nullptr;
    }
}

std::int32_t SceneTimer::Init()
{
    DCHECK(queue_ == nullptr);
    queue_ = new  assistx2::timer2::TimerQueue(ios_);
    if (queue_ == nullptr)
    {
        return -1;
    }

    return 0;
}

bool SceneTimer::CancelTimer(const SceneTimerContext & id)
{
    boost::shared_ptr<assistx2::timer2::TimerContext > context;
    return queue_->CancelTimer(id, context);
}

std::int32_t SceneTimer::NewTimer(long expires_from_now_millisecond, Trigger_type trigger,
    std::int32_t room, SceneTimerContext::TimerType type, Seat * seat)
{
    DCHECK_NE(type, SceneTimerContext::DELAY_BROADCAST_TIMER);

    boost::shared_ptr<SceneTimerContext> context(new EventTimerContext(trigger, room, type, seat));

    return queue_->NewTimer(context, expires_from_now_millisecond);
}

std::int32_t SceneTimer::NewTimer(long expires_from_now_millisecond, Trigger_type trigger,
    std::int32_t room, std::shared_ptr<assistx2::Stream> stream, DelayTimerContext::BroadCastType type, uid_type mid)
{
    boost::shared_ptr<SceneTimerContext> context(new DelayTimerContext(trigger, room, stream, type, mid));

    return queue_->NewTimer(context, expires_from_now_millisecond);
}

bool EventTimerContext::Equal(const TimerContext * other) const
{
    const EventTimerContext * other_context = dynamic_cast<const EventTimerContext *>(other);

    if (other_context == nullptr)
    {
        return false;
    }

    if (other_context->type_ != type_ || other_context->room_ != room_)
    {
        return false;
    }

    if (seat_ == nullptr)
    {
        DCHECK(other_context->seat_ == nullptr);
        return true;
    }
    else
    {
        return other_context->seat_->seat_no() == seat_->seat_no();
    }
}
