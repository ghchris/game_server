#ifndef FRAMEWORK_GAME_SEATDATA_H_
#define FRAMEWORK_GAME_SEATDATA_H_

#include <queue>
#include <memory>
#include "card.h"
#include "cardlogic.h"
#include <map>

enum OperCardsType
{
    NULL_TYPE = -1,
    PENG = 0,       //碰
    MGANG = 1,  //明杠
    AGANG = 2,   //暗杠
    GGANG = 3,   //公杠
};

class CardGroup;
class Data
{
public:
    Data() {}
    ~Data() {}
public:
    std::int64_t seat_score_ = 0;
    std::shared_ptr<CardGroup> hand_cards_;
    std::queue<std::shared_ptr< Card > > played_cards_;   //打出的牌
    std::shared_ptr< Card > mo_card_ = nullptr;                  //摸的牌
    std::vector<CardLogic::OperationType> now_operate_;//当前的操作
    std::map<std::shared_ptr< Card >,OperCardsType> operated_cards_;     //已操作的牌
};

#endif