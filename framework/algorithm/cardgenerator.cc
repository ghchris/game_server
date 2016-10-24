#include "cardgenerator.h"
#include <algorithm>

#define TEST_CARD

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
#ifdef TEST_CARD
    MakeTestCard();
#else
    std::random_shuffle(cards_.begin(), cards_.end());
#endif // _DEBUG
}

std::shared_ptr< Card > CardGenerator::Pop()
{
#ifndef TEST_CARD
    std::random_shuffle(cards_.begin(), cards_.end());
#endif
    auto card = cards_.back();
    cards_.pop_back();

    return card;
}

std::uint32_t CardGenerator::count() const
{
    return cards_.size();
}


void CardGenerator::MakeTestCard()
{
    std::vector<std::string> cards = {
        "1W","1W","6W","6W","6W","6W","1T","1T","8S","7W","7S","3S","4W","5S","7T",
        "7W","4W","2S","5S","8W","7W","9W","7W","9S","4T","9W","1W",
        "1S","4T","6S","2T","4S","8W","5S","4T","3T","6T","2T","3W","1S","5T",
        "8W","2S","1S","2W","9T","3T","6T","3W","2T","4S","7T","4W",
        "8T","9W","4S","6S","7S","7S","8W","9T","4S","5W",
        "9T","2W","3W","2W","4W","3S","7S","3W","3S","7T",
        "1T","3S","6T","5W","7T","8S","8T","1T","8S",
        "6T","9S","9S","9W","4T","5W","3T","5S","5T",
        "8S","5T","8T","3T","2S","5W","9S","6S","2S","2T",
        "5T","8T","2W","6S","9T","1S","1W","HZ","HZ","HZ","HZ",
    };
    cards_.clear();
    for (auto iter = cards.rbegin(); iter != cards.rend(); iter++)
    {
        auto card = CardFactory::MakeMajiangCard(*iter);
        cards_.push_back(card);
    }
}