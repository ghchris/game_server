#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "datalayer.h"
#include "robotagent.h"
#include <assistx2/tcphandler_wrapper.h>
#include "cardgroup.h"
#include "cardlogic.h"
#include "robotmanager.h"
#include "timerhelper.h"

const static std::int16_t SERVER_RESPONSE_ENTER_ROOM = 1001;
const static std::int16_t CLIENT_READY_CMD = 1005;
const static std::int16_t SERVER_BROADCAST_ON_DEAL = 1020;//发牌
const static std::int16_t SERVER_NOTIFY_OPERATE = 1022;//通知用户做相应的操作
const static std::int16_t CLIENT_REQUEST_ONPLAY = 1023;//用户出牌
const static std::int16_t CLIENT_REQUEST_ONPENG = 1024;//用户碰
const static std::int16_t CLIENT_REQUEST_ONGANG = 1025;//用户杠
const static std::int16_t CLIENT_REQUEST_ONHU = 1026;//用户胡
const static std::int16_t SERVER_NOTIFY_MO_CARD = 1028;//通知用户摸牌
const static std::int16_t SERVER_BROADCAST_GAME_ACCOUNT = 1031;//单局结算
const static std::int16_t SERVER_BROADCAST_HAS_BEEN_DISBAND = 1013;
const static std::int16_t NET_CONNECT_CLOSED = 1999;

class RobotAgentImpl
{
public:
    RobotAgentImpl(RobotAgent* owner);
    ~RobotAgentImpl();

    void OnEnterRoom(assistx2::Stream *packet);
    void OnDealCard(assistx2::Stream *packet);
    void OnOperation(assistx2::Stream *packet);
    void OnPeng(assistx2::Stream *packet);
    void OnGang(assistx2::Stream *packet);
    void OnMo(assistx2::Stream *packet);
    void OnGameOver(assistx2::Stream *packet);
public:
    void Ready();
    void Play();
    void Peng();
    void gang();
    void Hu();
    void Destroy();
public:
    RobotAgent* owner_;
    std::int32_t seatno_;
    std::shared_ptr<CardGroup> hand_cards_ = nullptr;
    MemberGame member_game_;
    MemberFides member_fides_;
    MemberCommonGame member_common_game_;
};

RobotAgent::RobotAgent():
 AgentBase(AgentType::ROBOT),
 pImpl_(new RobotAgentImpl(this))
{

}

RobotAgent::~RobotAgent()
{

}

bool RobotAgent::Serialize(bool loadorsave)
{
    if (loadorsave == true)
    {
        set_connect_status(true);

        if (DataLayer::getInstance()->membercommongame(uid(),
            pImpl_->member_common_game_) != 0)
        {
            DLOG(INFO) << "membercommongame failed mid:=" << uid();
            return false;
        }
        if (DataLayer::getInstance()->memberfides(uid(), pImpl_->member_fides_) != 0)
        {
            DLOG(INFO) << "memberfides failed mid:=" << uid();
            return false;
        }

//         if (DataLayer::getInstance()->membergame(uid(), pImpl_->member_game_) != 0)
//         {
//             //DLOG(INFO) << "membergame failed mid:=" << uid();
//             //return false;
//         }
    }
    else
    {
        set_connect_status(false);
        //now do nothing
    }

    return true;
}



void RobotAgent::SendTo(const assistx2::Stream& packet, bool needsave)
{
    if (connect_status() == true)
    {
        GlobalTimerProxy::getInstance()->NewTimer(
            std::bind(&RobotAgent::OnMessage, this, packet), 1);
    }
}

void RobotAgent::OnMessage(assistx2::Stream clone)
{
    auto cmd = clone.GetCmd();
    switch (cmd)
    {
    case SERVER_RESPONSE_ENTER_ROOM:
        pImpl_->OnEnterRoom(&clone);
        break;
    case SERVER_BROADCAST_ON_DEAL:
        pImpl_->OnDealCard(&clone);
        break;
    case SERVER_NOTIFY_OPERATE:
        pImpl_->OnOperation(&clone);
        break;
    case CLIENT_REQUEST_ONPENG:
        pImpl_->OnPeng(&clone);
        break;
    case CLIENT_REQUEST_ONGANG:
        pImpl_->OnGang(&clone);
        break;
    case SERVER_NOTIFY_MO_CARD:
        pImpl_->OnMo(&clone);
        break;
    case  SERVER_BROADCAST_GAME_ACCOUNT:
        pImpl_->OnGameOver(&clone);
        break;
    case SERVER_BROADCAST_HAS_BEEN_DISBAND:
        RobotManager::getInstance()->RecoverRobot(shared_from_this());
        pImpl_->Destroy();
        break;
    default:
        break;
    }
}

MemberCommonGame* RobotAgent::member_common_game()
{
    return &pImpl_->member_common_game_;
}

MemberFides* RobotAgent::member_fides()
{
    return &pImpl_->member_fides_;
}

MemberGame* RobotAgent::member_game()
{
    return &pImpl_->member_game_;
}

RobotAgentImpl::RobotAgentImpl(RobotAgent* owner):
 owner_(owner)
{
}

RobotAgentImpl::~RobotAgentImpl()
{
}

void RobotAgentImpl::Ready()
{
    assistx2::Stream stream(CLIENT_READY_CMD);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::Play()
{
    std::shared_ptr<Card> card = nullptr;
    auto card_info = hand_cards_->hand_cards_info();
    for (auto iter : card_info)
    {
        if (iter.num < 2)
        {
            card = iter.card;
        }
    }
    if (card == nullptr)
    {
        for (auto iter : card_info)
        {
            if (iter.card->getFace() == Card::Face::HongZ)
            {
                continue;;
            }
            card = iter.card;
            break;
        }
    }
    
    if (card == nullptr)
    {
        for (auto iter : card_info)
        {
            card = iter.card;
            break;
        }
    }

    DCHECK(card != nullptr);
    assistx2::Stream stream(CLIENT_REQUEST_ONPLAY);
    stream.Write(card->getName());
    stream.End();

    owner_->Process(&stream);
    hand_cards_->RemoveCard(card);
}

void RobotAgentImpl::Peng()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONPENG);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::gang()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONGANG);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::Hu()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONHU);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::OnEnterRoom(assistx2::Stream *packet)
{
    auto err = packet->Read<std::int32_t>();
    if (err <= 0)
    {
        return;
    }

    seatno_ = err;
    Ready();
}

void RobotAgentImpl::OnDealCard(assistx2::Stream *packet)
{
    Cards cards;
    auto size = packet->Read<int32_t>();
    for (auto i = 0; i < size; ++i)
    {
        auto cardname = packet->Read<std::string>();
        auto card = CardFactory::MakeMajiangCard(cardname);
        cards.push_back(card);
    }
    hand_cards_ = std::make_shared<CardGroup>(cards);
}

void RobotAgentImpl::OnOperation(assistx2::Stream *packet)
{
    /*auto seatno =*/ packet->Read<int32_t>();
    auto size = packet->Read<int32_t>();
    for (auto i = 0; i < size; ++i)
    {
        auto operation = packet->Read<int32_t>();
        if (operation == CardLogic::PLAY_OPERA)
        {
            Play();
            break;
        }
        else if (operation == CardLogic::HUPAI_OPERA)
        {
            Hu();
            break;
        }
        else if (operation == CardLogic::PENG_OPERA)
        {
            Peng();
            break;
        }
        else if (operation == CardLogic::GANG_OPERA)
        {
            gang();
            break;
        }
        else
        {
            DLOG(ERROR)<< "RobotAgentImpl::OnOperation() error operation:=" << operation;
        }
    }
}

void RobotAgentImpl::OnPeng(assistx2::Stream *packet)
{
    auto seatno = packet->Read<int32_t>();
    /*auto targetseatno =*/ packet->Read<int32_t>();
    auto cardname = packet->Read<std::string>();
   
    auto card = CardFactory::MakeMajiangCard(cardname);
    if (seatno == seatno_)
    {
        CHECK(hand_cards_->RemoveCard(card) == true);
        CHECK(hand_cards_->RemoveCard(card) == true);
    }
}

void RobotAgentImpl::OnGang(assistx2::Stream *packet)
{
    auto seatno = packet->Read<int32_t>();
    auto type = packet->Read<int32_t>();
    auto targetseatno = packet->Read<int32_t>();
    auto cardname = packet->Read<std::string>();
    auto card = CardFactory::MakeMajiangCard(cardname);
    if (seatno == seatno_)
    {
        if (seatno == targetseatno)
        {
            CHECK(hand_cards_->RemoveCard(card) == true);
        }
        if (type != 3)
        {
            CHECK(hand_cards_->RemoveCard(card) == true);
            CHECK(hand_cards_->RemoveCard(card) == true);
            CHECK(hand_cards_->RemoveCard(card) == true);
        }
    }
}

void RobotAgentImpl::OnMo(assistx2::Stream *packet)
{
    /*auto seatno =*/ packet->Read<int32_t>();
    auto cardname = packet->Read<std::string>();
    
    auto card = CardFactory::MakeMajiangCard(cardname);
    hand_cards_->AddCard(card);
}

void RobotAgentImpl::OnGameOver(assistx2::Stream *packet)
{
    Ready();
}

void RobotAgentImpl::Destroy()
{
    assistx2::Stream stream(NET_CONNECT_CLOSED);
    stream.End();

    owner_->Process(&stream);

    DCHECK(owner_->scene_object() == nullptr);
}