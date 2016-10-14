#include "majiang.h"
#include <iostream>

static const char face_symbols[] =
{
    '1', '2', '3', '4', '5', '6', '7', '8', '9','H'
};

static const char type_symbols[] =
{
    'T', 'S', 'W','Z'
};

Majiang::Majiang(Face face, Type  type)
 :CardBase(face,type)
{
    name_.resize(2, '\0');
    name_.at(0) = getFaceSymbol();
    name_.at(1) = getSuitSymbol();
}

Majiang::~Majiang()
{

}

char Majiang::getFaceSymbol() const
{
    return face_symbols[getFace() - Card::FirstFace];
}

char Majiang::getSuitSymbol() const
{
    return type_symbols[getType() - Card::FirstType];
}

std::string Majiang::getName() const
{
    return name_;
}

Card::Face Majiang::convertFaceSymbol(char fsym)
{
    for (unsigned int i = Card::FirstFace; i <= Card::LastFace; ++i)
    {
        if (fsym == face_symbols[i - Card::FirstFace])
        {
            return static_cast<Card::Face>(i);
        }
    }

    throw std::invalid_argument("");
}

Card::Type Majiang::convertTypeSymbol(char ssym)
{
    for (unsigned int i = Card::FirstType; i <= Card::LastType; ++i)
    {
        if (ssym == type_symbols[i - Card::FirstType])
        {
            return static_cast<Card::Type>(i);
        }
    }

    throw std::invalid_argument("");
}

std::shared_ptr<Card > CardFactory::MakeMajiangCard(Card::Face face, Card::Type type)
{
    std::shared_ptr<Card > card(new Majiang(face, type));

    return card;
}

std::shared_ptr<Card > CardFactory::MakeMajiangCard(const std::string & name)
{
    std::shared_ptr< Card > card = nullptr;

    try
    {
        Card::Face f = Majiang::convertFaceSymbol(name.at(0));
        Card::Type t = Majiang::convertTypeSymbol(name.at(1));

        card = std::shared_ptr<Card >(new Majiang(f, t));
    }
    catch (...)
    {
        std::cout << "CardFactory::MakePokerCard FAILED, invalid_argument:=" << name << std::endl;
    }

    return card;
}