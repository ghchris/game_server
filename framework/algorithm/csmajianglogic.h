#ifndef MAJIANG_GAME_CSMAJIANGLOGIC_H_
#define MAJIANG_GAME_CSMAJIANGLOGIC_H_

#include "cardlogicbase.h"

class CSMajiangLogicImpl;
class CSMajiangLogic :public CardLogicBase
{
public:
    CSMajiangLogic();
    virtual ~CSMajiangLogic();
private:
    std::unique_ptr< CSMajiangLogicImpl > pImpl_;
};

#endif
