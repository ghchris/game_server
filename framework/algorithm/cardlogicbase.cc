#include "cardlogicbase.h"
#include "cardgroup.h"

class CardLogicBaseImpl
{
public:
    CardLogicBaseImpl(CardLogicBase* owner);
    ~CardLogicBaseImpl();

    bool AnalyzeNormal(int[]);
    bool AnalyzeJiang(int[]);
    bool AnalyzeDui(int[],int hz);
public:
    CardLogicBase* owner_;
};


CardLogicBase::CardLogicBase():
 pImpl_(new CardLogicBaseImpl(this))
{

}

CardLogicBase::~CardLogicBase()
{

}

bool CardLogicBase::CheckPeng(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    if (cardgroup->card_count(card) == 2)
    {
        return true;
    }

    return false;
}

bool CardLogicBase::CheckGang(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    if (cardgroup->card_count(card) == 3)
    {
        return true;
    }

    return false;
}

bool CardLogicBase::CheckHu(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(card, cardgroup);

    return IsHu();
}

bool CardLogicBase::CheckHu7Dui(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(card, cardgroup);

    return is7Dui();
}

bool CardLogicBase::CheckTing(const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(nullptr, cardgroup);

    return IsTing();
}

bool CardLogicBase::CheckTing7Dui(const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(nullptr, cardgroup);

    return is7Dui(1);
}

void CardLogicBase::ChangeCardToArray(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    for (auto iter : cardgroup->hand_cards())
    {
        cards_array[iter->getType() - 1][0] += 1;
        cards_array[iter->getType() - 1][iter->getFace()] += 1;
    }

    if (card != nullptr)
    {
        cards_array[card->getType() - 1][0] += 1;
        cards_array[card->getType() - 1][card->getFace()] += 1;
    }
}

bool CardLogicBase::IsHu()
{
    int nJiangPos = -1;						//��������λ�� 
    int nMod = 0;							//���� 
    bool bJiangExisted = false;

    //�Ƿ�����3,3,3,3,2ģ�� 
    for (int i = 0; i < 3; i++)
    {
        nMod = cards_array[i][0] % 3;
        if (nMod == 1)
        {
            return false;
        }
        if (nMod == 2) {
            if (bJiangExisted)
            { //�����������ƵĿ��� ���ܹ��ɺ���
                return false;
            }
            nJiangPos = i;
            bJiangExisted = true;
        }
    }

    //�ǽ��������ж�
    for (int i = 0; i < 3; i++)
    {
        if (i != nJiangPos) {
            if (!pImpl_->AnalyzeNormal(cards_array[i]))
            {
                return false;
            }
        }
    }

    //��������Ҫ������,	��ΪҪ�Խ�������ѯ,	Ч�ʽϵ�,������� 
    return pImpl_->AnalyzeJiang(cards_array[nJiangPos]);
}

bool CardLogicBase::IsTing(std::int32_t need_card)
{
    auto bTing = false;

    for (int i = 0; i < 3; i++)
    {
        cards_array[i][0]++;
        for (int j = 1; j < 10; j++)
        {
            cards_array[i][j]++;
            if (need_card > 1)
            {
                if (IsTing(need_card - 1))
                {
                    return true;
                }
            }
            else
            {
                if (IsHu())
                {
                    return true;
                }
            }
            cards_array[i][j]--;
        }
        cards_array[i][0]--;
    }
    return bTing;
}

bool CardLogicBase::is7Dui(std::int32_t hz_card)
{
    for (int i = 0; i < 3; i++)
    {
        if (!pImpl_->AnalyzeDui(cards_array[i], hz_card))
        {
            return false;
        }
    }

    return true;
}

CardLogicBaseImpl::CardLogicBaseImpl(CardLogicBase* owner):
 owner_(owner)
{
}

CardLogicBaseImpl::~CardLogicBaseImpl()
{
}

/*
*��ͨ���ͷ���
*����1: ����������
*����2: �Ƿ�Ϊ����
*/
bool CardLogicBaseImpl::AnalyzeNormal(int aKindPai[])
{
    if (aKindPai[0] == 0)
    {
        return true;
    }

    //Ѱ�ҵ�һ���� 
    int j = -1;
    for (j = 1; j < 10; j++)
    {
        if (aKindPai[j] != 0)
        {
            break;
        }
    }

    bool result;
    if (aKindPai[j] >= 3)//��Ϊ���� 
    {
        //��ȥ��3�ſ��� 
        aKindPai[j] -= 3;
        aKindPai[0] -= 3;
        result = AnalyzeNormal(aKindPai);
        //��ԭ��3�ſ��� 
        aKindPai[j] += 3;
        aKindPai[0] += 3;
        return result;
    }

    //��Ϊ˳�� 
    if ((j < 8) && (aKindPai[j + 1] > 0) && (aKindPai[j + 2] > 0))
    {
        //��ȥ��3��˳�� 
        aKindPai[j]--;
        aKindPai[j + 1]--;
        aKindPai[j + 2]--;
        aKindPai[0] -= 3;
        result = AnalyzeNormal(aKindPai);

        //��ԭ��3��˳�� 
        aKindPai[j]++;
        aKindPai[j + 1]++;
        aKindPai[j + 2]++;
        aKindPai[0] += 3;
        return result;
    }
    return false;
}

bool CardLogicBaseImpl::AnalyzeDui(int aKindPai[],int hz)
{
    if (aKindPai[0] == 0)
    {
        return true;
    }

    int j = -1;
    for (j = 1; j < 10; j++)
    {
        if (aKindPai[j] != 0)
        {
            break;
        }
    }

    bool result;
    if (aKindPai[j] >= 2) 
    {
        aKindPai[j] -= 2;
        aKindPai[0] -= 2;
        result = AnalyzeDui(aKindPai, hz);
        aKindPai[j] += 2;
        aKindPai[0] += 2;
        return result;
    }

    if (aKindPai[j] == 1)
    {
        hz -= 1;
        if (hz >= 0)
        {
            aKindPai[j] -= 1;
            aKindPai[0] -= 1;
            result = AnalyzeDui(aKindPai, hz);
            aKindPai[j] += 1;
            aKindPai[0] += 1;
            return result;
        }
    }

    return false;
}

bool CardLogicBaseImpl::AnalyzeJiang(int aKindPai[])
{
    bool success = false;								//ָʾ�������������ܷ�ͨ�� 
    for (int j = 1; j < 10; j++) 					//���н��в���,��j��ʾ 
    {
        //�ҵ���������λ��
        if (aKindPai[j] >= 2)
        {
            //��ȥ��2�Ž��� 
            aKindPai[j] -= 2;
            aKindPai[0] -= 2;
            if (AnalyzeNormal(aKindPai))
            {
                success = true;
            }
            //��ԭ��2�Ž��� 
            aKindPai[j] += 2;
            aKindPai[0] += 2;
            if (success) break;
        }
    }

    return success;
}