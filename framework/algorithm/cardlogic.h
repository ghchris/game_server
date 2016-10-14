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
        PLAY_OPERA = 100,         //³öÅÆ²Ù×÷
        PENG_OPERA = 101,        //ÅöÅÆ²Ù×÷
        GANG_OPERA = 102,       //¸ÜÅÆ²Ù×÷
        HUPAI_OPERA = 103,      //ºúÅÆ²Ù×÷
    };
public:
    CardLogic() {}
    virtual ~CardLogic() {}

    virtual bool CheckPeng(const std::shared_ptr<Card> /**/, 
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckGang(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu7Dui(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckTing(const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckTing7Dui(const std::shared_ptr<CardGroup> /**/) = 0;
};

#endif
