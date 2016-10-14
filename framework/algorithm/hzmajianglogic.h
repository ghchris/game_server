#ifndef MAJIANG_GAME_HZMAJIANGLOGIC_H_
#define MAJIANG_GAME_HZMAJIANGLOGIC_H_

#include "cardlogicbase.h"

class HZMajiangLogicImpl;
class HZMajiangLogic :public CardLogicBase
{
public:
    HZMajiangLogic();
    virtual ~HZMajiangLogic();

    virtual bool CheckHu(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckHu7Dui(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
protected:
    virtual void ChangeCardToArray(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
private:
    std::unique_ptr< HZMajiangLogicImpl > pImpl_;
};

#endif //MAJIANG_GAME_HZMAJIANGLOGIC_H_