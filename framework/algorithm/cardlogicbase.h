#ifndef MAJIANG_GAME_CARDLOGICBASE_H_
#define MAJIANG_GAME_CARDLOGICBASE_H_

#include "cardlogic.h"

class CardLogicBaseImpl;
class CardLogicBase:public CardLogic
{
public:
    CardLogicBase();
    virtual ~CardLogicBase();

    virtual bool CheckChi(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckPeng(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup) ;
    virtual bool CheckGang(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckHu(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup) ;
    virtual bool CheckHu_258(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/);
    virtual bool CheckHu7Dui(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckTing(const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckTing_258(const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckTing7Dui(const std::shared_ptr<CardGroup> cardgroup);
protected:
    virtual void ChangeCardToArray(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    bool IsHu();
    bool IsTing(std::int32_t need_card = 1);
    bool is7Dui(std::int32_t hz_card = 0);
    bool IsHu_258();
    bool IsTing_258(std::int32_t need_card = 1);
public:
    std::int32_t cards_array[3][10] = {
        { 0,0,0,0,0,0,0,0,0,0 },               //Í² 
        { 0,0,0,0,0,0,0,0,0,0 },               //Ë÷
        { 0,0,0,0,0,0,0,0,0,0 }                //Íò 
    };
private:
    std::unique_ptr< CardLogicBaseImpl > pImpl_;
};

#endif
