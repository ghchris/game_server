#ifndef MAJIANG_GAME_CARDGENERATOR_H_
#define MAJIANG_GAME_CARDGENERATOR_H_

#include "card.h"

class CardGenerator
{
public:
    enum class Type {GENERAL_MAJIANG ,HZ_MAJIANG};
public:
    explicit CardGenerator();
    virtual ~CardGenerator();

    void Reset(Type type = Type::GENERAL_MAJIANG);
    std::shared_ptr< Card > Pop();
    std::uint32_t count() const;
protected:
    void MakeTestCard();
private:
    Cards cards_;
};

#endif
