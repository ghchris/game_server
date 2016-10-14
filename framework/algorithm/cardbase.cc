#include "cardbase.h"

CardBase::CardBase(Face face, Type  type):
 face_(face),
 type_(type)
{

}

CardBase::~CardBase()
{

}

Card::Face CardBase::getFace() const
{
    return face_;
}

Card::Type CardBase::getType() const
{
    return type_;
}