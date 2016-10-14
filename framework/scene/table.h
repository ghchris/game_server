#ifndef FRAMEWORK_SCENE_TABLE_H_
#define FRAMEWORK_SCENE_TABLE_H_

#include <memory>
#include <vector>
#include "define.h"

class Seat;
class Agent;
class TableImpl;
class Table
{
public:
    const static std::int32_t BEGIN_SEAT = (1);
    const static std::int32_t INVALID_SEAT = (0);
public:
    explicit Table(std::int32_t seat_count);
    virtual ~Table();

    //seatno默认为0时，自动分配座位
    std::int32_t Enter(std::shared_ptr<Agent> player, const std::int32_t seatno = INVALID_SEAT);
    std::int32_t Leave(std::shared_ptr<Agent> player);
    Seat* GetByUid(uid_type uid);
    Seat* GetBySeatNo(std::int32_t seatno);
    const std::vector< Seat* >& GetSeats() const;
    std::int32_t player_count();
private:
    DISALLOW_COPY_AND_ASSIGN(Table);
private:
    std::unique_ptr< TableImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_TABLE_H_