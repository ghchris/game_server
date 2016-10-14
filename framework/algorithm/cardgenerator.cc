#include "cardgenerator.h"
#include <algorithm>

CardGenerator::CardGenerator()
{

}

CardGenerator::~CardGenerator()
{

}

void CardGenerator::Reset(Type type)
{
    cards_.clear();

    for (std::int32_t f = Card::One; f <= Card::Nine; ++f)
    {
        for (std::int32_t t = Card::Tong; t <= Card::Wan; ++t)
        {
            for (size_t i = 0; i < 4; i++)
            {
                auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(f), static_cast<Card::Type>(t));
                cards_.push_back(card);
            }
        }
    }

    switch (type)
    {
    case CardGenerator::Type::GENERAL_MAJIANG:
        break;
    case CardGenerator::Type::HZ_MAJIANG:
    {
        for (size_t i = 0; i < 4; i++)
        {
            auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(Card::HongZ), static_cast<Card::Type>(Card::Zi));
            cards_.push_back(card);
        }
    }
        break;
    default:
        break;
    }
    std::random_shuffle(cards_.begin(), cards_.end());
}

std::shared_ptr< Card > CardGenerator::Pop()
{
    std::random_shuffle(cards_.begin(), cards_.end());
    auto card = cards_.back();
    cards_.pop_back();

    return card;
}

std::uint32_t CardGenerator::count() const
{
    return cards_.size();
}