#include "cardgroup.h"
#include <vector>
#include <algorithm>

class CardGroupImpl
{
public:
    CardGroupImpl();
    ~CardGroupImpl();
    void ParseCards();
    bool UpdateCardsNum(std::shared_ptr< Card > card, bool isAdd = true);
public:
    Cards handcards_;
    std::vector< CardGroup::CardInfo > cards_info_;
};

CardGroup::CardGroup(Cards cards) :
    pImpl_(new CardGroupImpl)
{
    pImpl_->handcards_ = cards;
    pImpl_->ParseCards();
}

CardGroup::~CardGroup()
{
    pImpl_->handcards_.clear();
    pImpl_->cards_info_.clear();
}

void CardGroup::AddCard(std::shared_ptr< Card > card)
{
    pImpl_->handcards_.push_back(card);
    pImpl_->UpdateCardsNum(card);
}

bool CardGroup::RemoveCard(std::shared_ptr< Card > card)
{
    auto iter = std::find_if(pImpl_->handcards_.begin(), pImpl_->handcards_.end(),
        [&card](const std::shared_ptr< Card > value) {
        return value->getFace() == card->getFace()
            && value->getType() == card->getType();
    });
    if (iter != pImpl_->handcards_.end())
    {
        pImpl_->handcards_.erase(iter);
    }
    else
    {
        return false;
    }

    return pImpl_->UpdateCardsNum(card, false);
}

std::int32_t CardGroup::card_count(std::shared_ptr< Card > card)
{
    auto iter = std::find_if(pImpl_->cards_info_.begin(), pImpl_->cards_info_.end(),
        [&card](const CardGroup::CardInfo& value) {
        return value.card->getFace() == card->getFace() &&
            value.card->getType() == card->getType();
    });
    if (iter != pImpl_->cards_info_.end())
    {
        return iter->num;
    }

    return 0;
}

const Cards& CardGroup::hand_cards() const
{
    return pImpl_->handcards_;
}

const std::vector< CardGroup::CardInfo >& CardGroup::hand_cards_info() const
{
    return pImpl_->cards_info_;
}

CardGroupImpl::CardGroupImpl()
{
}

CardGroupImpl::~CardGroupImpl()
{

}

void CardGroupImpl::ParseCards()
{
    for (auto iter : handcards_)
    {
        UpdateCardsNum(iter);
    }
}

bool CardGroupImpl::UpdateCardsNum(std::shared_ptr< Card > card, bool isAdd)
{
    auto card_iter = std::find_if(cards_info_.begin(), cards_info_.end(),
        [&card](const CardGroup::CardInfo& value) {
        return value.card->getFace() == card->getFace() &&
            value.card->getType() == card->getType();
    });
    if (isAdd == true)
    {
        if (card_iter != cards_info_.end())
        {
            card_iter->num += 1;
        }
        else
        {
            CardGroup::CardInfo card_info;
            card_info.card = card;
            card_info.num = 1;
            cards_info_.push_back(card_info);
        }
    }
    else
    {
        if (card_iter != cards_info_.end())
        {
            if (card_iter->num > 1)
            {
                card_iter->num -= 1;
            }
            else
            {
                cards_info_.erase(card_iter);
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}