#ifndef MAJIANG_GAME_CARD_H_
#define MAJIANG_GAME_CARD_H_

#include <string>
#include <vector>
#include <memory>

class Card
{
public:
    enum Face
    {
        FirstFace = 1,
        One = 1,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        HongZ,
        LastFace = HongZ
    };

    enum Type
    {
        FirstType = 1,
        Tong = 1,         //Ͳ
        Suo,                 //��
        Wan,                //��
        Zi,
        LastType = Zi
    };
public:
    Card() {}
    virtual ~Card() {}

    virtual Face getFace() const = 0;
    virtual Type getType() const = 0;

    virtual std::string getName() const = 0;
};

typedef std::vector<std::shared_ptr< Card > > Cards;

class CardFactory
{
public:
    static std::shared_ptr<Card > MakeMajiangCard(const std::string & name);

    static std::shared_ptr<Card > MakeMajiangCard(Card::Face face, Card::Type suit);
};

#endif //MAJIANG_GAME_CARD_H_
