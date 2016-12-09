#ifndef MAJIANG_GAME_ZZMAJIANGLOGIC_H_
#define MAJIANG_GAME_ZZMAJIANGLOGIC_H_

#include "cardlogicbase.h"

class ZZMajiangLogicImpl;
class ZZMajiangLogic :public CardLogicBase
{
public:
    ZZMajiangLogic();
    virtual ~ZZMajiangLogic();

    virtual bool CheckPeng(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckGang(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckHu(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckHu7Dui(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
protected:
    virtual void ChangeCardToArray(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
private:
    std::unique_ptr< ZZMajiangLogicImpl > pImpl_;
};

#endif //MAJIANG_GAME_ZZMAJIANGLOGIC_H_