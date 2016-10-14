#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include "hzmajiangroom.h"
#include "table.h"
#include "seat.h"
#include "cardgenerator.h"
#include "seatdata.h"
#include "cardlogic.h"
#include "agent.h"
#include "cardgroup.h"

const static std::int16_t SERVER_BROADCAST_ON_DEAL = 1020;//发牌
const static std::int16_t SERVER_BROADCAST_NEXT_PLAYER = 1021;//发牌
const static std::int16_t SERVER_NOTIFY_OPERATE = 1021;//通知用户做相应的操作
const static std::int16_t CLIENT_OPERATION = 1022;//用户做相应的操作

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
public:
    void OnOperation(std::shared_ptr<Agent > player, assistx2::Stream * packet);
public:
    HzMajiangRoom* owner_;
    std::int32_t banker_seatno_ = Table::INVALID_SEAT;//庄家座位号
    std::int32_t now_operator_ = Table::INVALID_SEAT;//当前操作座位号
    std::int32_t now_played_seatno_ = Table::INVALID_SEAT;//当前打出牌的座位号
};

HzMajiangRoom::HzMajiangRoom(std::uint32_t seat_count, 
    std::uint32_t id, std::string type):
 PrivateRoom(seat_count,id,type),
 pImpl_(new HzMajiangRoomImpl(this))
{

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
    case CLIENT_OPERATION:
        pImpl_->OnOperation(player, packet);
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

        DCHECK(iter->player != nullptr);
        iter->player->SendTo(package);

        iter->data()->hand_cards_ = std::make_shared<CardGroup>(cards);
    }

     pImpl_->BroadCastNextPlayer(pImpl_->banker_seatno_);

     pImpl_->NotifyOperation(pImpl_->banker_seatno_,
         std::vector<CardLogic::OperationType>(CardLogic::PLAY_OPERA));
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
    now_operator_ = seatno;

    assistx2::Stream package(SERVER_BROADCAST_NEXT_PLAYER);
    package.Write(seatno);
    package.End();

    owner_->BroadCast(package);
}

void HzMajiangRoomImpl::NotifyOperation(std::int32_t seatno,
    std::vector<CardLogic::OperationType> operate)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);
    DCHECK(seat->player != nullptr);

    assistx2::Stream package(SERVER_NOTIFY_OPERATE);
    package.Write(seatno);
    package.Write(static_cast<std::int32_t>(operate.size()));
    for (auto iter : operate)
    {
        package.Write(static_cast<std::int32_t>(iter));
        DLOG(INFO) << "NotifyOperation: mid:=" << seat->player->uid()
            << ",operate:=" << iter;
    }
    package.End();

    seat->player->SendTo(package);

    seat->data()->now_operate_ = operate;
}

void HzMajiangRoomImpl::OnOperation(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto operate = static_cast<CardLogic::OperationType>
        (packet->Read<std::int32_t>());

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if ( !CheckOperation(seat,operate) )
    {
        DLOG(ERROR) << "OnOperation:CheckOperation Failed mid:=" << player->uid() 
            << ",operate:="<< operate;
        SendErrorCode(player, CLIENT_OPERATION, -1);
        return;
    }

    if (operate == CardLogic::PLAY_OPERA)
    {
        auto card_name = packet->Read<std::string>();
        auto card = CardFactory::MakeMajiangCard(card_name);
        if (card == nullptr)
        {
            DLOG(ERROR) << "OnOperation:PlayCard Failed mid:=" << player->uid()
                << ",card_name:=" << card_name;
            SendErrorCode(player, CLIENT_OPERATION, -1);
            return;
        }
        seat->data()->hand_cards_->RemoveCard(card);
        seat->data()->played_cards_.push(card);
    }

    SendErrorCode(player, CLIENT_OPERATION, 0);
}

bool HzMajiangRoomImpl::CheckOperation(Seat* seat, CardLogic::OperationType operate)
{
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

}

void HzMajiangRoomImpl::SendErrorCode(std::shared_ptr<Agent > player,
    const std::int16_t cmd, const std::int32_t err)
{
    assistx2::Stream stream(cmd);
    stream.Write(err);
    stream.End();

    player->SendTo(stream);
}