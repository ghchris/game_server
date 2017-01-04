#ifndef MAJIANG_GAME_CARDLOGIC_H_
#define MAJIANG_GAME_CARDLOGIC_H_

#include <memory>

class Card;
class CardGroup;
class CardLogic
{
public:
    enum OperationType
    {
        NULL_OPERA = -1,
        PLAY_OPERA = 100,         //出牌操作
        PENG_OPERA = 101,        //碰牌操作
        GANG_OPERA = 102,       //杠牌操作
        HUPAI_OPERA = 103,      //胡牌操作
        CANCLE_OPERA = 104,    //取消操作
        MO_OPERA = 105,           //摸牌操作
        CHI_OPERA = 106,           //吃牌操作
        BU_OPERA = 107,            //补张操作
    };
public:
    CardLogic() {}
    virtual ~CardLogic() {}

    virtual bool CheckChi(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckPeng(const std::shared_ptr<Card> /**/, 
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckGang(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu_258(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu7Dui(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckTing(const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckTing_258(const std::shared_ptr<CardGroup> cardgroup) = 0;
    virtual bool CheckTing7Dui(const std::shared_ptr<CardGroup> /**/) = 0;
};

#endif
