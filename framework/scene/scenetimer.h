#ifndef _FRAMEWORK_SRC_ROOM_TIMER_H_
#define _FRAMEWORK_SRC_ROOM_TIMER_H_

#include <assistx2/timer2.h>
#include <assistx2/stream.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include "define.h"

class Seat;

//trigger
typedef boost::function<void(boost::shared_ptr<assistx2::timer2::TimerContext >) > Trigger_type;

class SceneTimerContext : public assistx2::timer2::TimerContext
{
public:
    enum TimerType
    {
        DELAY_BROADCAST_TIMER,//延迟广播时间
        MAJIANG_MOPAI,               //摸牌
        MAJIANG_BUPAI,                 //补张
        MAJIANG_GANG_BUPAI,     //杠补张
        MAJIANG_GAMEOVER,       //游戏结束
        MAJIANG_DISBAND,          //解散房间
    };

public:
    SceneTimerContext(const Trigger_type & trigger, std::int32_t room, TimerType type)
        :TimerContext(trigger), room_(room), type_(type)
    {

    }

    virtual ~SceneTimerContext()
    {

    }

    std::int32_t room_;
    TimerType type_;
};

class EventTimerContext : public SceneTimerContext
{
public:
    EventTimerContext(const Trigger_type & trigger, std::int32_t room, TimerType type, Seat * seat)
        :SceneTimerContext(trigger, room, type), seat_(seat)
    {

    }

    virtual ~EventTimerContext() {}

    virtual bool Equal(const TimerContext * right)const;

    Seat * seat_;
};

class DelayTimerContext : public SceneTimerContext
{
public:
    enum BroadCastType
    {
        BROADCAST_TO_ALL,
        BROADCAST_TO_VISITOR,
        BROADCAST_TO_TARGET,
        BROADCAST_TO_INGAME
    };

    DelayTimerContext(const Trigger_type & trigger, std::int32_t room, std::shared_ptr<assistx2::Stream > msg, BroadCastType type, uid_type mid = 0)
        :SceneTimerContext(trigger, room, DELAY_BROADCAST_TIMER), msg_(msg), broadcast_type_(type), mid_(mid)
    {

    }

    virtual ~DelayTimerContext() {}

    virtual bool Equal(const TimerContext * /*right*/)const
    {
        return false;
    }

    std::shared_ptr<assistx2::Stream > msg_;

    BroadCastType broadcast_type_;

    uid_type mid_;
};

class SceneTimer
{
    SceneTimer(void);
    SceneTimer(const SceneTimer &);
    SceneTimer & operator=(const SceneTimer &);
public:
    explicit SceneTimer(boost::asio::io_service & ios);
    ~SceneTimer(void);

    std::int32_t Init();

    std::int32_t NewTimer(long expires_from_now_millisecond, Trigger_type trigger,
        std::int32_t scene, SceneTimerContext::TimerType type, Seat * seat);

    std::int32_t NewTimer(long expires_from_now_millisecond, Trigger_type trigger,
        std::int32_t scene, std::shared_ptr<assistx2::Stream > stream, DelayTimerContext::BroadCastType type, uid_type mid = 0);

    bool CancelTimer(const SceneTimerContext & id);

private:
    boost::asio::io_service & ios_;

    assistx2::timer2::TimerQueue * queue_;
};

#endif //_FRAMEWORK_SRC_ROOM_TIMER_H_

