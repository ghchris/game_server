#ifndef MAJIANG_GAME_CARDBASE_H_
#define MAJIANG_GAME_CARDBASE_H_

#include "card.h"

class CardBase:public Card
{
public:
    CardBase(Face face, Type  type);
    virtual ~CardBase();
    virtual Face getFace() const;
    virtual Type getType() const;
private:
    Face face_;//大小
    Type type_;//类型

protected:
    std::string name_;
};

#endif
