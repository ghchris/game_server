#ifndef MAJIANG_GAME_CARDLOGIC_H_
#define MAJIANG_GAME_CARDLOGIC_H_

#include <memory>

class Card;
class CardGroup;
class CardLogic
{
public:
    enum OperationType
    {
        NULL_OPERA = -1,
        PLAY_OPERA = 100,         //���Ʋ���
        PENG_OPERA = 101,        //���Ʋ���
        GANG_OPERA = 102,       //���Ʋ���
        HUPAI_OPERA = 103,      //���Ʋ���
        CANCLE_OPERA = 104,    //ȡ������
        MO_OPERA = 105,           //���Ʋ���
        CHI_OPERA = 106,           //���Ʋ���
        BU_OPERA = 107,            //���Ų���
    };
public:
    CardLogic() {}
    virtual ~CardLogic() {}

    virtual bool CheckChi(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckPeng(const std::shared_ptr<Card> /**/, 
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckGang(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu_258(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckHu7Dui(const std::shared_ptr<Card> /**/,
        const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckTing(const std::shared_ptr<CardGroup> /**/) = 0;
    virtual bool CheckTing_258(const std::shared_ptr<CardGroup> cardgroup) = 0;
    virtual bool CheckTing7Dui(const std::shared_ptr<CardGroup> /**/) = 0;
};

#endif
