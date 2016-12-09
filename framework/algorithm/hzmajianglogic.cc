#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "hzmajianglogic.h"
#include "cardgroup.h"

class HZMajiangLogicImpl
{
public:
    HZMajiangLogicImpl();
    ~HZMajiangLogicImpl();
public:
    std::int32_t hz_count_ = 0;
};

HZMajiangLogic::HZMajiangLogic():
 CardLogicBase(),
 pImpl_(new HZMajiangLogicImpl)
{

}

HZMajiangLogic::~HZMajiangLogic()
{

}

bool HZMajiangLogic::CheckHu(const std::shared_ptr<Card> card,
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

bool HZMajiangLogic::CheckHu7Dui(const std::shared_ptr<Card> card,
    const std::shared_ptr<CardGroup> cardgroup)
{
    ChangeCardToArray(card, cardgroup);

    return is7Dui(pImpl_->hz_count_);
}

void HZMajiangLogic::ChangeCardToArray(const std::shared_ptr<Card> card,
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

HZMajiangLogicImpl::HZMajiangLogicImpl()
{
}

HZMajiangLogicImpl::~HZMajiangLogicImpl()
{
}