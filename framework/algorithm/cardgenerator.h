#ifndef MAJIANG_GAME_CARDGENERATOR_H_
#define MAJIANG_GAME_CARDGENERATOR_H_

#include "card.h"

class CardGenerator
{
public:
    enum class Type {GENERAL_MAJIANG ,HZ_MAJIANG,ZZ_TWO_PLAYER};
public:
    explicit CardGenerator();
    virtual ~CardGenerator();

    void Reset(Type type = Type::GENERAL_MAJIANG);
    std::shared_ptr< Card > Pop();
    std::shared_ptr< Card > Pop(Cards cards);
    std::uint32_t count() const;
protected:
    void MakeTestCard();
    void MakeGeneralCard();
    void MakeHongZhongCard();
    void MakeZZTwoPlayerCard();
private:
    Cards cards_;
    Type type_;
};

#endif
