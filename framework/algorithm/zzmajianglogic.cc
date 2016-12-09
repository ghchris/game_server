#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "zzmajianglogic.h"
#include "cardgroup.h"

class ZZMajiangLogicImpl
{
public:
    ZZMajiangLogicImpl();
    ~ZZMajiangLogicImpl();
public:
    std::int32_t hz_count_ = 0;
};

ZZMajiangLogic::ZZMajiangLogic():
 CardLogicBase(),
 pImpl_(new ZZMajiangLogicImpl)
{

}

ZZMajiangLogic::~ZZMajiangLogic()
{

}

bool ZZMajiangLogic::CheckPeng(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    if (card->getName() == "HZ")
    {
        return false;
    }
    if (cardgroup->card_count(card) >= 2)
    {
        return true;
    }

    return false;
}

bool ZZMajiangLogic::CheckGang(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    if (card == nullptr)
    {
        auto cards_info = cardgroup->hand_cards_info();
        for (auto iter : cards_info)
        {
            if (iter.card->getName() == "HZ")
            {
                continue;
            }
            if (iter.num == 4)
            {
                return true;
            }
        }
    }
    else
    {
        if (card->getName() == "HZ")
        {
            return false;
        }
        if (cardgroup->card_count(card) == 3)
        {
            return true;
        }
    }

    return false;
}

bool ZZMajiangLogic::CheckHu(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(card, cardgroup);

    DLOG(INFO) << "CheckHu cards:" << cardgroup->hand_cards()
        << ",hz_count_:=" << pImpl_->hz_count_;

    if (pImpl_->hz_count_ == 0)
    {
        return IsHu();
    }
    else
    {
        return IsTing(pImpl_->hz_count_);
    }

    return false;
}

bool ZZMajiangLogic::CheckHu7Dui(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(card, cardgroup);

    return is7Dui(pImpl_->hz_count_);
}

void ZZMajiangLogic::ChangeCardToArray(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    memset(cards_array, 0, sizeof cards_array);

    pImpl_->hz_count_ = 0;
    for (auto iter : cardgroup->hand_cards())
    {
        if (iter->getType() == Card::Type::Zi &&
             iter->getFace() == Card::Face::HongZ)
        {
            pImpl_->hz_count_ += 1;
            continue;
        }
        cards_array[iter->getType() - 1][0] += 1;
        cards_array[iter->getType() - 1][iter->getFace()] += 1;
    }

    if (card == nullptr)
    {
        return;
    }

    if (card->getType() == Card::Type::Zi)
    {
        pImpl_->hz_count_ += 1;
    }
    else
    {
        cards_array[card->getType() - 1][0] += 1;
        cards_array[card->getType() - 1][card->getFace()] += 1;
    }
}

ZZMajiangLogicImpl::ZZMajiangLogicImpl()
{
}

ZZMajiangLogicImpl::~ZZMajiangLogicImpl()
{
}