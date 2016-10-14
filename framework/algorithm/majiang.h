#ifndef MAJIANG_GAME_HZMAJIANG_H_
#define MAJIANG_GAME_HZMAJIANG_H_

#include "cardbase.h"

class Majiang:public CardBase
{
public:
    Majiang(Face face, Type  type);
    virtual ~Majiang();

    char getFaceSymbol() const;
    char getSuitSymbol() const;

    virtual std::string getName() const;

    static Face convertFaceSymbol(char fsym);
    static Type convertTypeSymbol(char ssym);
};

#endif
