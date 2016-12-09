#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "zzmajianglogic_twoplayer.h"
#include "cardgroup.h"

class ZZMajiangLogicTwoPlayerImpl
{
public:
    ZZMajiangLogicTwoPlayerImpl();
    ~ZZMajiangLogicTwoPlayerImpl();
    void SetCardToArray(std::shared_ptr<Card> card);
    bool AnalyzeNormal(int[]);
    bool AnalyzeNormal_Ex(int[]);
    bool AnalyzeJiang(int[]);
    bool AnalyzeJiang_Ex(int[]);
    bool AnalyzeDui(int[]);
    bool IsHu();
    bool Is7Dui();
public:
    std::int32_t cards_array_[3][10] = {
        { 0,0,0,0,0,0,0,0,0,0 },               //万 
        { 0,0,0,0,0,0,0,0,0,0 },               //字牌
        { 0,0,0,0,0,0,0,0,0,0 }                //风牌
    };
};

ZZMajiangLogicTwoPlayer::ZZMajiangLogicTwoPlayer() :
    CardLogicBase(),
    pImpl_(new ZZMajiangLogicTwoPlayerImpl)
{

}

ZZMajiangLogicTwoPlayer::~ZZMajiangLogicTwoPlayer()
{

}

bool ZZMajiangLogicTwoPlayer::CheckHu(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(card, cardgroup);

    DLOG(INFO) << "CheckHu cards:" << cardgroup->hand_cards();

    return pImpl_->IsHu();
}

bool ZZMajiangLogicTwoPlayer::CheckHu7Dui(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(card, cardgroup);

    return pImpl_->Is7Dui();
}

void ZZMajiangLogicTwoPlayer::ChangeCardToArray(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    memset(pImpl_->cards_array_, 0, sizeof pImpl_->cards_array_);

    for (auto iter : cardgroup->hand_cards())
    {
        pImpl_->SetCardToArray(iter);
    }

    if (card == nullptr)
    {
        return;
    }
    
    pImpl_->SetCardToArray(card);
}

ZZMajiangLogicTwoPlayerImpl::ZZMajiangLogicTwoPlayerImpl()
{
}

ZZMajiangLogicTwoPlayerImpl::~ZZMajiangLogicTwoPlayerImpl()
{
}

void ZZMajiangLogicTwoPlayerImpl::SetCardToArray(std::shared_ptr<Card> card)
{
    if (card->getType() == Card::Type::Wan)
    {
        cards_array_[0][0] += 1;
        cards_array_[0][card->getFace()] += 1;
    }
    else if (card->getType() == Card::Type::Zi)
    {
        cards_array_[1][0] += 1;
        cards_array_[1][card->getFace() - Card::Face::HongZ + 1] += 1;
    }
    else if (card->getType() == Card::Type::Feng)
    {
        cards_array_[2][0] += 1;
        cards_array_[2][card->getFace() - Card::Face::DongF + 1] += 1;
    }
    else
    {
        DCHECK(false) << "ChangeCardToArray:type error card:=" << card->getName();
    }
}

bool ZZMajiangLogicTwoPlayerImpl::IsHu()
{
    int nJiangPos = -1;						//“将”的位置 
    int nMod = 0;							//余数 
    bool bJiangExisted = false;

    //是否满足3,3,3,3,2模型 
    for (int i = 0; i < 3; i++)
    {
        nMod = cards_array_[i][0] % 3;
        if (nMod == 1)
        {
            return false;
        }
        if (nMod == 2) {
            if (bJiangExisted)
            { //存在两个将牌的可能 不能构成胡牌
                return false;
            }
            nJiangPos = i;
            bJiangExisted = true;
        }
    }

    //非将牌类型判断
    for (int i = 0; i < 3; i++)
    {
        if (i != nJiangPos) 
        {
            if (i == 0)
            {
                if (!AnalyzeNormal(cards_array_[i]))
                {
                    return false;
                }
            }
            else
            {
                if (!AnalyzeNormal_Ex(cards_array_[i]))
                {
                    return false;
                }
            }
        }
    }

    //该类牌中要包含将,	因为要对将进行轮询,	效率较低,放在最后
    if (nJiangPos == 0)
    {
        return AnalyzeJiang(cards_array_[nJiangPos]);
    }
    else
    {
        return AnalyzeJiang_Ex(cards_array_[nJiangPos]);
    }
    
}

bool ZZMajiangLogicTwoPlayerImpl::Is7Dui()
{
    for (int i = 0; i < 3; i++)
    {
        if (!AnalyzeDui(cards_array_[i]))
        {
            return false;
        }
    }

    return true;
}

/*
*普通牌型分析
*参数1: 类型牌数组
*参数2: 是否为字牌
*/
bool ZZMajiangLogicTwoPlayerImpl::AnalyzeNormal(int aKindPai[])
{
    if (aKindPai[0] == 0)
    {
        return true;
    }

    //寻找第一张牌 
    int j = -1;
    for (j = 1; j < 10; j++)
    {
        if (aKindPai[j] != 0)
        {
            break;
        }
    }

    bool result;
    if (aKindPai[j] >= 3)//作为刻牌 
    {
        //除去这3张刻牌 
        aKindPai[j] -= 3;
        aKindPai[0] -= 3;
        result = AnalyzeNormal(aKindPai);
        //还原这3张刻牌 
        aKindPai[j] += 3;
        aKindPai[0] += 3;
        return result;
    }

    //作为顺牌 
    if ((j < 8) && (aKindPai[j + 1] > 0) && (aKindPai[j + 2] > 0))
    {
        //除去这3张顺牌 
        aKindPai[j]--;
        aKindPai[j + 1]--;
        aKindPai[j + 2]--;
        aKindPai[0] -= 3;
        result = AnalyzeNormal(aKindPai);

        //还原这3张顺牌 
        aKindPai[j]++;
        aKindPai[j + 1]++;
        aKindPai[j + 2]++;
        aKindPai[0] += 3;
        return result;
    }
    return false;
}

bool ZZMajiangLogicTwoPlayerImpl::AnalyzeNormal_Ex(int aKindPai[])
{
    if (aKindPai[0] == 0)
    {
        return true;
    }

    //寻找第一张牌 
    int j = -1;
    for (j = 1; j < 10; j++)
    {
        if (aKindPai[j] != 0)
        {
            break;
        }
    }

    bool result;
    if (aKindPai[j] >= 3)//作为刻牌 
    {
        //除去这3张刻牌 
        aKindPai[j] -= 3;
        aKindPai[0] -= 3;
        result = AnalyzeNormal_Ex(aKindPai);
        //还原这3张刻牌 
        aKindPai[j] += 3;
        aKindPai[0] += 3;
        return result;
    }

    return false;
}

bool ZZMajiangLogicTwoPlayerImpl::AnalyzeDui(int aKindPai[])
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
        result = AnalyzeDui(aKindPai);
        aKindPai[j] += 2;
        aKindPai[0] += 2;
        return result;
    }

    return false;
}

bool ZZMajiangLogicTwoPlayerImpl::AnalyzeJiang(int aKindPai[])
{
    bool success = false;								//指示除掉“将”后能否通过 
    for (int j = 1; j < 10; j++) 					//对列进行操作,用j表示 
    {
        //找到将牌所在位序
        if (aKindPai[j] >= 2)
        {
            //除去这2张将牌 
            aKindPai[j] -= 2;
            aKindPai[0] -= 2;
            if (AnalyzeNormal(aKindPai))
            {
                success = true;
            }
            //还原这2张将牌 
            aKindPai[j] += 2;
            aKindPai[0] += 2;
            if (success) break;
        }
    }

    return success;
}

bool ZZMajiangLogicTwoPlayerImpl::AnalyzeJiang_Ex(int aKindPai[])
{
    bool success = false;								//指示除掉“将”后能否通过 
    for (int j = 1; j < 10; j++) 					//对列进行操作,用j表示 
    {
        //找到将牌所在位序
        if (aKindPai[j] >= 2)
        {
            //除去这2张将牌 
            aKindPai[j] -= 2;
            aKindPai[0] -= 2;
            if (AnalyzeNormal_Ex(aKindPai))
            {
                success = true;
            }
            //还原这2张将牌 
            aKindPai[j] += 2;
            aKindPai[0] += 2;
            if (success) break;
        }
    }

    return success;
}