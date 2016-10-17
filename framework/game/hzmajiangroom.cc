#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include "hzmajiangroom.h"
#include "table.h"
#include "seat.h"
#include "cardgenerator.h"
#include "seatdata.h"
#include "hzmajianglogic.h"
#include "agent.h"
#include "cardgroup.h"

const static std::int16_t SERVER_BROADCAST_ON_DEAL = 1020;//发牌
const static std::int16_t SERVER_BROADCAST_NEXT_PLAYER = 1021;//发牌
const static std::int16_t SERVER_NOTIFY_OPERATE = 1021;//通知用户做相应的操作
const static std::int16_t CLIENT_REQUEST_ONPLAY = 1022;//用户出牌
const static std::int16_t SERVER_BROADCAST_PLAYED_CARD = 1022;//广播打出的牌
const static std::int16_t CLIENT_REQUEST_ONPENG = 1023;//用户碰
const static std::int16_t SERVER_BROADCAST_PENG_CARD = 1023;//广播碰
const static std::int16_t CLIENT_REQUEST_ONGANG= 1024;//用户杠
const static std::int16_t SERVER_BROADCAST_GANG_CARD = 1024;//广播杠
const static std::int16_t CLIENT_REQUEST_ONHU = 1025;//用户胡
const static std::int16_t SERVER_BROADCAST_HU_CARD = 1025;//广播胡
const static std::int16_t CLIENT_REQUEST_ONCANCLE = 1026;//用户取消
const static std::int16_t SERVER_NOTIFY_MO_CARD = 1027;//通知用户摸牌
const static std::int16_t SERVER_BROADCAST_QIANGGANG_HU = 1028;//抢杠胡

class HzMajiangRoomImpl
{
public:
    HzMajiangRoomImpl(HzMajiangRoom* owner);
    ~HzMajiangRoomImpl();
    void BroadCastNextPlayer(std::int32_t seatno);
    void NotifyOperation(std::int32_t seatno,
        std::vector<CardLogic::OperationType> operate);
    bool CheckOperation(Seat* seat, CardLogic::OperationType operate);
    void SendErrorCode(std::shared_ptr<Agent > player, const std::int16_t cmd, const std::int32_t err);
    std::vector<CardLogic::OperationType> GetOperations(std::int32_t seatno);
    void BroadCastPlayedCard(std::int32_t seatno, std::shared_ptr<Card> card);
    void FindOperatorPlayer(std::int32_t seatno, std::shared_ptr<Card> card);
    bool FindGongGang(Seat* seat, std::shared_ptr<Card> card);
    bool FindQiangGangHu(std::int32_t seatno, std::shared_ptr<Card> card);
    void MoCard(std::shared_ptr<Agent > player);
public:
    void OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet);
public:
    HzMajiangRoom* owner_;
    std::int32_t banker_seatno_ = Table::INVALID_SEAT;//庄家座位号
    std::int32_t active_player_ = Table::INVALID_SEAT;//当前激活的玩家座位号(房间里的玩家都能看见)
    std::int32_t now_operator_ = Table::INVALID_SEAT;//当前操作座位号(只有自己才能看见)
    std::int32_t now_played_seatno_ = Table::INVALID_SEAT;//当前打出牌的座位号
    std::shared_ptr<CardLogic> card_logic_;
    std::vector<std::int32_t> hu_players_;
};

HzMajiangRoom::HzMajiangRoom(std::uint32_t seat_count, 
    std::uint32_t id, std::string type):
 PrivateRoom(seat_count,id,type),
 pImpl_(new HzMajiangRoomImpl(this))
{
    pImpl_->card_logic_ = std::make_shared<HZMajiangLogic>();
}

HzMajiangRoom::~HzMajiangRoom()
{

}

std::int32_t HzMajiangRoom::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto cmd = packet->GetCmd();
    DLOG(INFO) << "HzMajiangRoom::OnMessage()->cmd:" << cmd << " Scene:=" << scene_id()
        << " Scene Type:=" << scene_type() << " mid:=" << player->uid();

    switch (cmd)
    {
    case CLIENT_REQUEST_ONPLAY:
        pImpl_->OnPlay(player, packet);
        return 0;
    case CLIENT_REQUEST_ONPENG:
        pImpl_->OnPeng(player, packet);
        return 0;
    case CLIENT_REQUEST_ONGANG:
        pImpl_->OnGang(player, packet);
        return 0;
    case CLIENT_REQUEST_ONHU:
        pImpl_->OnHu(player, packet);
        return 0;
    case CLIENT_REQUEST_ONCANCLE:
        pImpl_->OnCancle(player, packet);
        return 0;
    default:
        return PrivateRoom::OnMessage(player,packet);
    }

    return 0;
}

void HzMajiangRoom::OnGameStart()
{
    if (pImpl_->banker_seatno_ == Table::INVALID_SEAT)
    {
        auto seat = table_obj()->GetByUid(room_owner());
        DCHECK(seat != nullptr);
        pImpl_->banker_seatno_ = seat->seat_no();
    }

    auto& seats = table_obj()->GetSeats();
    for (auto iter : seats)
    {
        Cards cards;
        assistx2::Stream package(SERVER_BROADCAST_ON_DEAL);
        if (pImpl_->banker_seatno_ == iter->seat_no())
        {
            package.Write(14);
            auto card = card_generator()->Pop();
            package.Write(card->getName());
            cards.push_back(card);
        }
        else
        {
            package.Write(13);
        }
        for (std::int32_t i = 0; i < 13; ++i)
        {
            auto card = card_generator()->Pop();
            package.Write(card->getName());
            cards.push_back(card);
        }
       
        package.End();

        DCHECK(iter->player() != nullptr);
        iter->player()->SendTo(package);

        iter->data()->hand_cards_ = std::make_shared<CardGroup>(cards);
    }

     pImpl_->BroadCastNextPlayer(pImpl_->banker_seatno_);

     auto operations = pImpl_->GetOperations(pImpl_->banker_seatno_);
     if (operations.size() == 0)
     {
         operations.push_back(CardLogic::PLAY_OPERA);
     }

     pImpl_->NotifyOperation(pImpl_->banker_seatno_,operations);
}

void HzMajiangRoom::OnGameOver()
{
    
}


HzMajiangRoomImpl::HzMajiangRoomImpl(HzMajiangRoom* owner) :
    owner_(owner)
{

}

HzMajiangRoomImpl::~HzMajiangRoomImpl()
{
}

void HzMajiangRoomImpl::BroadCastNextPlayer(std::int32_t seatno)
{
    active_player_ = seatno;

    assistx2::Stream package(SERVER_BROADCAST_NEXT_PLAYER);
    package.Write(seatno);
    package.End();

    owner_->BroadCast(package);
}

void HzMajiangRoomImpl::NotifyOperation(std::int32_t seatno,
    std::vector<CardLogic::OperationType> operate)
{
    now_operator_ = seatno;

    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);
    DCHECK(seat->player() != nullptr);

    assistx2::Stream package(SERVER_NOTIFY_OPERATE);
    package.Write(seatno);
    package.Write(static_cast<std::int32_t>(operate.size()));
    for (auto iter : operate)
    {
        package.Write(static_cast<std::int32_t>(iter));
        DLOG(INFO) << "NotifyOperation: mid:=" << seat->player()->uid()
            << ",operate:=" << iter;
    }
    package.End();

    seat->player()->SendTo(package);

    seat->data()->now_operate_ = operate;
}

void HzMajiangRoomImpl::OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    if (active_player_ != now_operator_)
    {
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::PLAY_OPERA))
    {
        DLOG(ERROR) << "OnPlay:CheckOperation Failed mid:=" << player->uid()
            << ",operate:=" << CardLogic::PLAY_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }
   
    auto card_name = packet->Read<std::string>();
    auto card = CardFactory::MakeMajiangCard(card_name);
    if (card == nullptr)
    {
        DLOG(ERROR) << "OnOperation:PlayCard Failed mid:=" << player->uid()
            << ",card_name:=" << card_name;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }
    seat->data()->hand_cards_->RemoveCard(card);
    seat->data()->played_cards_.push(card);

    now_played_seatno_ = seatno;

    BroadCastPlayedCard(now_played_seatno_, card);

    FindOperatorPlayer(now_played_seatno_, card);
}

void HzMajiangRoomImpl::OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    if (active_player_ == now_operator_)
    {
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::PENG_OPERA))
    {
        DLOG(ERROR) << "OnPeng:CheckOperation Failed mid:=" << player->uid()
            << ",operate:=" << CardLogic::PENG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    DCHECK(now_played_seatno_ != Table::INVALID_SEAT);
    auto played_seat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
    DCHECK(played_seat != nullptr);

    auto card = played_seat->data()->played_cards_.front();
    played_seat->data()->played_cards_.pop();
    DCHECK(card_logic_->CheckPeng(card, seat->data()->hand_cards_) == true);
    CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
    CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);

    seat->data()->operated_cards_.insert(std::make_pair(card, PENG));

    assistx2::Stream stream(SERVER_BROADCAST_PENG_CARD);
    stream.Write(seatno);
    stream.Write(played_seat->seat_no());
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);


    BroadCastNextPlayer(seatno);
    
    NotifyOperation(seatno, std::vector<CardLogic::OperationType>(CardLogic::PLAY_OPERA));
}

void HzMajiangRoomImpl::OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::GANG_OPERA))
    {
        DLOG(ERROR) << "OnGang:CheckOperation Failed mid:=" << player->uid()
            << ",operate:=" << CardLogic::GANG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    OperCardsType type = NULL_TYPE;
    std::shared_ptr<Card> card = nullptr;
    Seat* operaseat = nullptr;
    if (active_player_ == now_operator_)
    {
        operaseat = seat;
        type = AGANG;
        auto cards_info = seat->data()->hand_cards_->hand_cards_info();
        for (auto iter : cards_info)
        {
            if (iter.num == 4)
            {
                card = iter.card;
                break;
            }
        }
        DCHECK(card != nullptr);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
    }
    else
    {
        DCHECK(now_played_seatno_ != Table::INVALID_SEAT);
        auto played_seat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
        DCHECK(played_seat != nullptr);

        operaseat = played_seat;

        auto card = played_seat->data()->played_cards_.front();
        played_seat->data()->played_cards_.pop();
        for (auto iter : seat->data()->operated_cards_)
        {
            if (iter.second != CardLogic::PENG_OPERA)
            {
                continue;
            }
            if (iter.first->getFace() == card->getFace())
            {
                type = GGANG;
                seat->data()->operated_cards_.erase(iter.first);
                break;
            }
        }

        if (type == NULL_TYPE)
        {
            DCHECK(card_logic_->CheckGang(card, seat->data()->hand_cards_) == true);
            type = MGANG;
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        }
    }

    seat->data()->operated_cards_.insert(std::make_pair(card, type));

    assistx2::Stream stream(SERVER_BROADCAST_GANG_CARD);
    stream.Write(seatno);
    stream.Write(static_cast<std::int32_t>(type));
    stream.Write(operaseat->seat_no());
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);

    if (type == GGANG)
    {
        auto res = FindQiangGangHu(seatno,card);
        if (res == true) return;
    }

    BroadCastNextPlayer(seatno);

    MoCard(player);
}

void HzMajiangRoomImpl::OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    if (active_player_ == now_operator_)
    {
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::HUPAI_OPERA))
    {
        DLOG(ERROR) << "OnPeng:CheckOperation Failed mid:=" << player->uid()
            << ",operate:=" << CardLogic::HUPAI_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    assistx2::Stream stream(SERVER_BROADCAST_HU_CARD);
    stream.Write(seatno);
    stream.End();

    owner_->BroadCast(stream);

    hu_players_.push_back(seatno);

    owner_->OnGameOver();
}
void HzMajiangRoomImpl::OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::CANCLE_OPERA))
    {
        DLOG(ERROR) << "OnCancle:CheckOperation Failed mid:=" << player->uid()
            << ",operate:=" << CardLogic::CANCLE_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    DCHECK(active_player_ != Table::INVALID_SEAT);
    auto nextseat = owner_->table_obj()->next_seat(active_player_);
    DCHECK(nextseat != nullptr);

    BroadCastNextPlayer(nextseat->seat_no());

    MoCard(nextseat->player);
}

bool HzMajiangRoomImpl::CheckOperation(Seat* seat, CardLogic::OperationType operate)
{
    for (auto iter : seat->data()->now_operate_)
    {
        if (iter == CardLogic::OperationType::PLAY_OPERA &&
            operate != iter)
        {
            return false;
        }
    }

    if (operate == CardLogic::CANCLE_OPERA)
    {
        seat->data()->now_operate_.clear();
        return true;
    }

    auto iter = std::find_if(seat->data()->now_operate_.begin(), seat->data()->now_operate_.end(), 
        [&operate](const CardLogic::OperationType value) {
        return value == operate;
    });
    auto res = true;
    if (iter != seat->data()->now_operate_.end())
    {
        res = false;
    }

    seat->data()->now_operate_.clear();

    return res;
}

std::vector<CardLogic::OperationType> HzMajiangRoomImpl::GetOperations(std::int32_t seatno)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    std::vector<CardLogic::OperationType> operatios;

    auto res = card_logic_->CheckHu(nullptr, seat->data()->hand_cards_);
    if (res == true)
    {
        operatios.push_back(CardLogic::HUPAI_OPERA);
    }
    res = card_logic_->CheckGang(nullptr, seat->data()->hand_cards_);
    if (res == true)
    {
        operatios.push_back(CardLogic::GANG_OPERA);
    }

    return operatios;
}

void HzMajiangRoomImpl::BroadCastPlayedCard(std::int32_t seatno, std::shared_ptr<Card> card)
{
    assistx2::Stream stream(SERVER_BROADCAST_PLAYED_CARD);
    stream.Write(seatno);
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);
}

void HzMajiangRoomImpl::FindOperatorPlayer(std::int32_t seatno, std::shared_ptr<Card> card)
{
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->seat_no() == seatno)
        {
            continue;
        }
        std::vector<CardLogic::OperationType> operatios;
        auto res = card_logic_->CheckPeng(card, iter->data()->hand_cards_);
        if (res == true)
        {
            operatios.push_back(CardLogic::PENG_OPERA);
        }
        res = card_logic_->CheckGang(card, iter->data()->hand_cards_);
        if (res == true)
        {
            operatios.push_back(CardLogic::GANG_OPERA);
        }
        if (operatios.size() == 0u)
        {
            auto nextseat = owner_->table_obj()->next_seat(seatno);
            BroadCastNextPlayer(nextseat->seat_no());

            MoCard(nextseat->player());
        }
        else
        {
            NotifyOperation(seatno,operatios);
        }
    }
}

bool HzMajiangRoomImpl::FindGongGang(Seat* seat, std::shared_ptr<Card> card)
{
    for (auto iter : seat->data()->operated_cards_)
    {
        if (iter.second != CardLogic::PENG_OPERA)
        {
            continue;
        }
        if (iter.first->getFace() == card->getFace())
        {
            return true;
        }
    }
    return false;
}

bool HzMajiangRoomImpl::FindQiangGangHu(std::int32_t seatno, std::shared_ptr<Card> card)
{
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->seat_no() == seatno)
        {
            continue;
        }
        auto res = card_logic_->CheckHu(card, iter->data()->hand_cards_);
        if (res == true)
        {
            hu_players_.push_back(iter->seat_no());
            assistx2::Stream stream(SERVER_BROADCAST_QIANGGANG_HU);
            stream.Write(iter->seat_no());
            stream.Write(seatno);
            stream.Write(card->getName());
            stream.End();

            owner_->BroadCast(stream);
        }
    }

    if (hu_players_.size() > 0u)
    {
        owner_->OnGameOver();
        return true;
    }

    return false;
}

void HzMajiangRoomImpl::MoCard(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto card = owner_->card_generator()->Pop();
    assistx2::Stream package(SERVER_NOTIFY_MO_CARD);
    package.Write(seatno);
    package.Write(card->getName());
    package.End();

    player->SendTo(package);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    seat->data()->mo_card_ = card;
    seat->data()->hand_cards_->AddCard(card);

    auto operations = GetOperations(seatno);
    auto res = FindGongGang(seat, card);
    if (res == true)
    {
        operations.push_back(CardLogic::GANG_OPERA);
    }
    if (operations.size() == 0)
    {
        operations.push_back(CardLogic::PLAY_OPERA);
    }
    NotifyOperation(seatno, operations);
}

void HzMajiangRoomImpl::SendErrorCode(std::shared_ptr<Agent > player,
    const std::int16_t cmd, const std::int32_t err)
{
    assistx2::Stream stream(cmd);
    stream.Write(err);
    stream.End();

    player->SendTo(stream);
}