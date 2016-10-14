#ifndef FRAMEWORK_SCENE_SEAT_H_
#define FRAMEWORK_SCENE_SEAT_H_

#include <memory>
#include "define.h"

class Data;
class Agent;
class SeatImpl;
class Seat
{
public:
    enum class SeatState{EMPTY,USING};

    //玩家等待
    const static std::uint32_t PLAYER_STATUS_WAITING = (1 << 0);
    //准备
    const static std::uint32_t PLAYER_STATUS_READY = (1 << 1);
    //在玩
    const static std::uint32_t PLAYER_STATUS_PLAYING = (1 << 2);
    //玩家掉线
    const static std::uint32_t PLAYER_STATUS_NET_CLOSE = (1 << 3);
public:
    explicit Seat(std::int32_t no);
    virtual ~Seat();

    //存取玩家ID
    void set_player(std::shared_ptr<Agent > player);
    const std::shared_ptr<Agent > player() const;

    //存取座位状态
    void set_seat_state(const SeatState& state);
    const SeatState& seat_state() const;

    //存取座位上玩家状态
    void set_seat_player_state(const std::uint32_t state);
    const std::uint32_t seat_player_state();

    const std::int32_t seat_no() const;

    Data* data();
private:
    DISALLOW_COPY_AND_ASSIGN(Seat);
private:
    std::unique_ptr< SeatImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_SEAT_H_