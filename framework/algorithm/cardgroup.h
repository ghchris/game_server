#ifndef MAJIANG_GAME_CARDGROUP_H_
#define MAJIANG_GAME_CARDGROUP_H_

#include "card.h"
#include <memory>
#include <map>

class CardGroupImpl;
class CardGroup
{
public:
    struct CardInfo
    {
        std::shared_ptr< Card > card;
        std::int32_t num;
    };
public:
    explicit CardGroup(Cards cards);
    virtual ~CardGroup();

    void AddCard(std::shared_ptr< Card > card);
    bool RemoveCard(std::shared_ptr< Card > card);

    std::int32_t card_count(std::shared_ptr< Card > card);
    const Cards& hand_cards() const;
    const std::vector< CardInfo >& hand_cards_info() const;
private:
    std::unique_ptr< CardGroupImpl > pImpl_;
};

#endif
