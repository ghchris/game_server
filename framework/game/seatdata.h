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
    PENG = 0,       //��
    MGANG = 1,  //����
    AGANG = 2,   //����
    GGANG = 3,   //����
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
    std::queue<std::shared_ptr< Card > > played_cards_;   //�������
    std::shared_ptr< Card > mo_card_ = nullptr;                  //������
    std::vector<CardLogic::OperationType> now_operate_;//��ǰ�Ĳ���
    std::map<std::shared_ptr< Card >,OperCardsType> operated_cards_;     //�Ѳ�������
};

#endif