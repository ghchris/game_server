#ifndef MAJIANG_GAME_ZZMAJIANGLOGIC_TWOPLAYER_H_
#define MAJIANG_GAME_ZZMAJIANGLOGIC_TWOPLAYER_H_

#include "cardlogicbase.h"

class ZZMajiangLogicTwoPlayerImpl;
class ZZMajiangLogicTwoPlayer :public CardLogicBase
{
public:
    ZZMajiangLogicTwoPlayer();
    virtual ~ZZMajiangLogicTwoPlayer();

    virtual bool CheckHu(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
    virtual bool CheckHu7Dui(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
protected:
    virtual void ChangeCardToArray(const std::shared_ptr<Card> card,
        const std::shared_ptr<CardGroup> cardgroup);
private:
    std::unique_ptr< ZZMajiangLogicTwoPlayerImpl > pImpl_;
};

#endif //MAJIANG_GAME_ZZMAJIANGLOGIC_H_