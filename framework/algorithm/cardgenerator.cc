#include "cardgenerator.h"
#include <algorithm>
#include "testcardmanager.h"

//#define TEST_CARD

CardGenerator::CardGenerator()
{

}

CardGenerator::~CardGenerator()
{

}

void CardGenerator::Reset(Type type)
{
    cards_.clear();
    type_ = type;

    switch (type)
    {
    case CardGenerator::Type::GENERAL_MAJIANG:
        MakeGeneralCard();
        break;
    case CardGenerator::Type::HZ_MAJIANG:
        MakeHongZhongCard();
        break;
    case  CardGenerator::Type::ZZ_TWO_PLAYER:
        MakeZZTwoPlayerCard();
        break;
    default:
        break;
    }
#ifdef TEST_CARD
    MakeTestCard();
    //TestCardManager::getInstance()->GetCustomCard(cards_);
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

std::shared_ptr< Card > CardGenerator::Pop(Cards cards)
{
    auto card = Pop();

    if (type_ == Type::ZZ_TWO_PLAYER)
    {
        return card;
    }

#ifndef TEST_CARD
    if (card->getFace() == Card::Face::HongZ &&
        card->getType() == Card::Type::Zi)
    {
        auto hzcount = 0;
        for (auto iter : cards)
        {
            if (iter->getFace() == Card::Face::HongZ &&
                card->getType() == Card::Type::Zi)
            {
                hzcount += 1;
            }
        }
        if (hzcount >= 2)
        {
            cards_.push_back(card);
            return Pop();
        }
    }
#endif

    return card;
}

std::uint32_t CardGenerator::count() const
{
    return cards_.size();
}

void CardGenerator::MakeHongZhongCard()
{
    for (size_t i = 0; i < 4; i++)
    {
        for (std::int32_t f = Card::One; f <= Card::Nine; ++f)
        {
            for (std::int32_t t = Card::Tong; t <= Card::Wan; ++t)
            {
                auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(f), static_cast<Card::Type>(t));
                cards_.push_back(card);
            }
        }

        auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(Card::HongZ), static_cast<Card::Type>(Card::Zi));
        cards_.push_back(card);
    }
}

void CardGenerator::MakeZZTwoPlayerCard()
{
    for (size_t i = 0; i < 4; i++)
    {
        for (std::int32_t f = Card::One; f <= Card::Nine; ++f)
        {
            auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(f), static_cast<Card::Type>(Card::Type::Wan));
            cards_.push_back(card);
        }
        std::random_shuffle(cards_.begin(), cards_.end());
        for (std::int32_t f = Card::HongZ; f <= Card::BaiZ; ++f)
        {
            auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(f), static_cast<Card::Type>(Card::Type::Zi));
            cards_.push_back(card);
        }
        std::random_shuffle(cards_.begin(), cards_.end());
        for (std::int32_t f = Card::DongF; f <= Card::BeiF; ++f)
        {
            auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(f), static_cast<Card::Type>(Card::Type::Feng));
            cards_.push_back(card);
        }
        std::random_shuffle(cards_.begin(), cards_.end());
    }
}

void CardGenerator::MakeGeneralCard()
{
    for (size_t i = 0; i < 4; i++)
    {
        for (std::int32_t f = Card::One; f <= Card::Nine; ++f)
        {
            for (std::int32_t t = Card::Tong; t <= Card::Wan; ++t)
            {
                auto card = CardFactory::MakeMajiangCard(static_cast<Card::Face>(f), static_cast<Card::Type>(t));
                cards_.push_back(card);
            }
        }
    }
}

void CardGenerator::MakeTestCard()
{
//     std::vector<std::string> cards = {
//         "HZ","HZ","HZ","HZ","7W","7W","7W","9W","9W","9W","9W""9S","9S","5S","6S","1T","4W","5W","6W",
//         "6W","6W","6W","1T","1T","8S","6S","6S","6T","2T","3W","1S","1T",
//         "3W","4W","5W","2S","2S","2S","7S","8S","9S","2T","3T","8T","8T",
//         "1W","1W","1W","7T","6T","5T","6S","4S","5S","3T","2T","5W","5W",
//         "2S","7S","2W","3W","4W","3T","8T","8T","7W",
//         "4T","1S","4T","4S","8W","5S","5T",
//         "8W","9T","3T","2T","7T",
//         "4W","7S","9S","1S","3S",
//         "4S","8W","9T","4S","5T","8W","4T",
//         "9T","3S","7S","3S","7T","2W",
//         "3S","7T","8S",
//         "6T","4T","5S",
//         "8S",
//         "5T","2W","9T","1S","1W","2W",
//     };
    //抢杠胡牌型 6S起手 其它摸到什么打什么
//     std::vector<std::string> cards = {
//         "HZ","HZ","4W","5W","6W","7W","7W","7W","9W","9W","9W","9S","9S","6S",
//         "6W","6W","6W","1T","1T","8S","7W","6S","6S","6T","2T","3W","1S",
//         "HZ","4W","5W","2S","2S","2S","7S","8S","4T","3T","5T","8T","8T",
//         "HZ","2S","4S","5S","7S","2W","3W","4W","3T","2T","1T","8T","8T",
//         "1W","7T","5T","6S","4W","3T","7W","9W"
//         "4T","1S","4T","4S","8W","5S","5T",
//         "8W","9T","3T","2T","7T",
//         "4W","7S","9S","1S","3S",
//         "4S","8W","9T","4S","5T","8W","4T",
//         "9T","3S","7S","3S","7T","2W",
//        "3S","7T","8S",
//         "6T","4T","5S",
//         "8S",
//         "5T","2W","9T","1S","1W","2W","HZ","HZ","HZ",
//     };
//     std::vector<std::string> cards = {
//         "4W","5W","6W","7W","7W","7W","9W","9W","9W","9S","9S","5S","6S","1T",
//         "6W","6W","6W","1T","1T","8S","6S","6S","6T","2T","3W","1S","1T",
//         "3W","4W","5W","2S","2S","2S","7S","8S","9S","2T","3T","8T","8T",
//         "1W","1W","1W","7T","6T","5T","6S","4S","5S","3T","2T","5W","5W",
//         "2S","7S","2W","3W","4W","3T","8T","8T","7W","9W"
//         "4T","1S","4T","4S","8W","5S","5T",
//         "8W","9T","3T","2T","7T",
//         "4W","7S","9S","1S","3S",
//         "4S","8W","9T","4S","5T","8W","4T",
//         "9T","3S","7S","3S","7T","2W",
//         "3S","7T","8S",
//         "6T","4T","5S",
//         "8S",
//         "5T","2W","9T","1S","1W","2W","HZ","HZ","HZ","HZ",
//     };
    std::vector<std::string> cards = {
        "9T","8T","2T","9W","1W","9S","7S","6S","4S","HZ","HZ","HZ",
        "4W","5W","6W","7W","7W","7W","9W","9W","9W","9S","9S","5S","6S","1T",
        "6W","6W","6W","1T","1T","8S","6S","6S","6T","2T","3W","1S","1T",
        "3W","4W","5W","2S","2S","2S","7S","8S","9S","2T","3T","8T","8T",
        "1W","1W","1W","7T","6T","5T","6S","4S","5S","3T","2T","5W","5W",
        "2S","7S","2W","3W","4W","3T","8T","8T","7W","9W"
        "4T","1S","4T","4S","8W","5S","5T",
        "8W","9T","3T","2T","7T",
        "4W","7S","9S","1S","3S",
        "4S","8W","9T","4S","5T","8W","4T",
        "9T","3S","7S","3S","7T","2W",
        "3S","7T","8S",
        "6T","4T","5S",
        "8S",
        "5T","2W","9T","1S","1W","2W","HZ","HZ","HZ","HZ",
    };
    cards_.clear();
    for (auto iter = cards.rbegin(); iter != cards.rend(); iter++)
    {
        auto card = CardFactory::MakeMajiangCard(*iter);
        cards_.push_back(card);
    }
}