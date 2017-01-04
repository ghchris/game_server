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
const static std::int16_t CLIENT_REQUEST_ONCANCLE = 1027;//用户取消
const static std::int16_t CLIENT_REQUEST_ONBU = 1037;//用户补张
const static std::int16_t SERVER_BROADCAST_BU_CARD = 1037;//用户补张
const static std::int16_t SERVER_NOTIFY_BU_OPERATION = 1038;//服务器通知杠后补张的操作
const static std::int16_t CLIENT_REQUEST_BU_OPERATION = 1039;
const static std::int16_t SERVER_RESPONSE_BU_OPERATION = 1039;
const static std::int16_t SERVER_NOTIFY_MT_OPERATION = 1042;//服务器通知起手明堂操作
const static std::int16_t CLIENT_REQUEST_MINGTANG_OPERATION = 1043;
const static std::int16_t SERVER_RESPONSE_MINGTANG_OPERATION = 1043;
const static std::int16_t SERVER_NOTIFY_HAIDIMANYOU = 1044;//服务器通知海底漫游
const static std::int16_t CLIENT_REQUEST_HAIDIMANYOU = 1044;//用户请求海底漫游
const static std::int16_t SERVER_BROADCAST_HAIDI_CARD = 1045;//服务器广播海底牌
const static std::int16_t SERVER_BROADCAST_HAIDI_HU = 1046;//海底胡
const static std::int16_t SERVER_BROADCAST_HAIDI_PAO = 1047;//海底胡

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
    void OnBu(assistx2::Stream *packet);
    void OnGangBu(assistx2::Stream *packet);
    void OnBuOperation(assistx2::Stream *packet);
    void OnMingtangOperation(assistx2::Stream *packet);
    void OnHaiDIManYou(assistx2::Stream *packet);
public:
    void Ready();
    void Play();
    void Peng();
    void gang();
    void Hu();
    void Cancel();
    void Destroy();
    void Bu();
    void BuOperation(int32_t operation,std::string card);
    void MTOperation(int32_t operation);
public:
    RobotAgent* owner_;
    std::int32_t seatno_;
    std::shared_ptr<CardGroup> hand_cards_ = nullptr;
    MemberGame member_game_;
    MemberFides member_fides_;
    MemberCommonGame member_common_game_;
    std::string operation_id_;
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
    case SERVER_BROADCAST_BU_CARD:
        pImpl_->OnBu(&clone);
        break;
    case SERVER_NOTIFY_BU_OPERATION:
        pImpl_->OnGangBu(&clone);
        break;
    case SERVER_RESPONSE_BU_OPERATION:
        pImpl_->OnBuOperation(&clone);
        break;
    case SERVER_NOTIFY_MT_OPERATION:
        pImpl_->OnMingtangOperation(&clone);
        break;
    case SERVER_NOTIFY_HAIDIMANYOU:
        pImpl_->OnHaiDIManYou(&clone);
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
    stream.Write(operation_id_);
    stream.End();

    owner_->Process(&stream);
    hand_cards_->RemoveCard(card);
}

void RobotAgentImpl::Peng()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONPENG);
    stream.Write(operation_id_);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::gang()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONGANG);
    stream.Write(std::string());
    stream.Write(operation_id_);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::Bu()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONBU);
    stream.Write(std::string());
    stream.Write(operation_id_);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::BuOperation(int32_t operation, std::string card)
{
    assistx2::Stream stream(CLIENT_REQUEST_BU_OPERATION);
    stream.Write(operation);
    stream.Write(card);
    stream.Write(operation_id_);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::MTOperation(int32_t operation)
{
    assistx2::Stream stream(CLIENT_REQUEST_MINGTANG_OPERATION);
    stream.Write(operation_id_);
    stream.Write(operation);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::Cancel()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONCANCLE);
    stream.Write(operation_id_);
    stream.End();

    owner_->Process(&stream);
}

void RobotAgentImpl::Hu()
{
    assistx2::Stream stream(CLIENT_REQUEST_ONHU);
    stream.Write(operation_id_);
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
    std::vector<std::int32_t> operations;
    for (auto i = 0; i < size; ++i)
    {
        auto operation = packet->Read<int32_t>();
        operations.push_back(operation);
    }
    operation_id_ = packet->Read<std::string>();

    for (auto iter : operations)
    {
        if (iter == CardLogic::PLAY_OPERA)
        {
            Play();
            break;
        }
        else if (iter == CardLogic::HUPAI_OPERA)
        {
            Hu();
            break;
        }
        else if (iter == CardLogic::GANG_OPERA)
        {
            gang();
            break;
        }
        else if (iter == CardLogic::BU_OPERA)
        {
            Bu();
        }
        else if (iter == CardLogic::PENG_OPERA)
        {
            Peng();
            break;
        }
        else if (iter == CardLogic::CHI_OPERA)
        {
            Cancel();
            break;
        }
        else
        {
            DLOG(ERROR) << "RobotAgentImpl::OnOperation() error operation:=" << iter;
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

void RobotAgentImpl::OnBu(assistx2::Stream *packet)
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
        if (type != 7)
        {
            CHECK(hand_cards_->RemoveCard(card) == true);
            CHECK(hand_cards_->RemoveCard(card) == true);
            CHECK(hand_cards_->RemoveCard(card) == true);
        }
    }
}

void RobotAgentImpl::OnGangBu(assistx2::Stream *packet)
{
    operation_id_ = packet->Read<std::string>();
    /*auto gang_seatno =*/ packet->Read<int32_t>();
    /*auto gang_card =*/ packet->Read<std::string>();
    auto card_count = packet->Read<int32_t>();
    for (auto i = 0; i < card_count; ++i)
    {
        auto cardname = packet->Read<std::string>();
        auto operation_count = packet->Read<int32_t>();
        for (auto j = 0; j < operation_count; ++j)
        {
            auto operation = packet->Read<int32_t>();
            BuOperation(operation, cardname);
            return;
        }
    }
}

void RobotAgentImpl::OnBuOperation(assistx2::Stream *packet)
{
    auto err = packet->Read<int32_t>();
    auto operation = packet->Read<int32_t>();
    auto card_name = packet->Read<std::string>();
    auto gang_seatno = packet->Read<int32_t>();
    if (err > 0 && err == seatno_ && gang_seatno == seatno_)
    {
        if (operation == 102 || operation == 103 || operation == 107)
        {
            auto card = CardFactory::MakeMajiangCard(card_name);
            hand_cards_->AddCard(card);
        }
    }
}

void RobotAgentImpl::OnMingtangOperation(assistx2::Stream *packet)
{
    /*auto seatno =*/ packet->Read<int32_t>();
    auto size = packet->Read<int32_t>();
    std::vector<std::int32_t> operations;
    for (auto i = 0; i < size; ++i)
    {
        auto operation = packet->Read<int32_t>();
        operations.push_back(operation);
    }
    operation_id_ = packet->Read<std::string>();

    MTOperation(204);
}

void RobotAgentImpl::OnHaiDIManYou(assistx2::Stream *packet)
{
    operation_id_ = packet->Read<std::string>();

    assistx2::Stream stream(CLIENT_REQUEST_HAIDIMANYOU);
    stream.Write(1);
    stream.Write(operation_id_);
    stream.End();

    owner_->Process(&stream);
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

    //DCHECK(owner_->scene_object() == nullptr);
}


