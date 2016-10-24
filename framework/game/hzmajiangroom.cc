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
#include "datalayer.h"

const static std::int16_t SERVER_BROADCAST_ON_DEAL = 1020;//发牌
const static std::int16_t SERVER_BROADCAST_NEXT_PLAYER = 1021;//下个出牌人
const static std::int16_t SERVER_NOTIFY_OPERATE = 1022;//通知用户做相应的操作
const static std::int16_t CLIENT_REQUEST_ONPLAY = 1023;//用户出牌
const static std::int16_t SERVER_BROADCAST_PLAYED_CARD = 1023;//广播打出的牌
const static std::int16_t CLIENT_REQUEST_ONPENG = 1024;//用户碰
const static std::int16_t SERVER_BROADCAST_PENG_CARD = 1024;//广播碰
const static std::int16_t CLIENT_REQUEST_ONGANG= 1025;//用户杠
const static std::int16_t SERVER_BROADCAST_GANG_CARD = 1025;//广播杠
const static std::int16_t CLIENT_REQUEST_ONHU = 1026;//用户胡
const static std::int16_t SERVER_BROADCAST_HU_CARD = 1026;//广播胡
const static std::int16_t CLIENT_REQUEST_ONCANCLE = 1027;//用户取消
const static std::int16_t SERVER_NOTIFY_MO_CARD = 1028;//通知用户摸牌
const static std::int16_t SERVER_BROADCAST_QIANGGANG_HU = 1029;//抢杠胡
const static std::int16_t SERVER_BROADCAST_ZHA_NIAO = 1030;//扎鸟
const static std::int16_t SERVER_BROADCAST_GAME_ACCOUNT = 1031;//单局结算
const static std::int16_t SERVER_NOTIFY_PLAYER_TABLE_DATA = 1032; //发送玩家桌面数据
const static std::int16_t SERVER_BROADCAST_PLAYER_MO = 1033;//服务器广播哪个玩家摸了牌
const static std::int16_t SERVER_BROADCAST_PLAYER_HANDCARD = 1034;//服务器广播玩家手上的牌数

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
    void CalculateScore(std::int32_t seatno, std::int32_t score);
    void ZhaNiao();
    bool CheckHu(const std::shared_ptr<Card> card,Seat* seat);
    std::int32_t GetZhongNiaoNum(std::int32_t seatno);
    void ClearGameData();
    void ClearRoomData();
    void NotifyTableData(std::shared_ptr<Agent > player);
public:
    void OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet);
public:
    HzMajiangRoom* owner_;
    std::int32_t active_player_ = Table::INVALID_SEAT;//当前激活的玩家座位号(房间里的玩家都能看见)
    std::int32_t now_operator_ = Table::INVALID_SEAT;//当前操作座位号(只有自己才能看见)
    std::int32_t now_played_seatno_ = Table::INVALID_SEAT;//当前打出牌的座位号
    std::int32_t last_mo_seat_ = Table::INVALID_SEAT;//最后摸牌的座位号
    std::int32_t beiqiang_seat_ = Table::INVALID_SEAT;//被抢杠的座位号
    std::shared_ptr<CardLogic> card_logic_;
    std::vector<std::int32_t> hu_players_;
    std::int32_t zhama_num_ = 2;
    std::map<std::int32_t, std::vector<std::shared_ptr<Card>>> zhong_ma_seats_;
    std::shared_ptr<Card> hu_card_ = nullptr;//胡的牌
    bool is_support_7dui = false;
    std::vector<std::int32_t> zhama_value_{ 2,3,4 };
};

HzMajiangRoom::HzMajiangRoom(std::uint32_t id, std::string type):
 PrivateRoom(id,type),
 pImpl_(new HzMajiangRoomImpl(this))
{
    pImpl_->card_logic_ = std::make_shared<HZMajiangLogic>();
}

HzMajiangRoom::~HzMajiangRoom()
{
    
}

void HzMajiangRoom::set_zhama_num(const std::int32_t value)
{
    auto iter = std::find_if(pImpl_->zhama_value_.begin(), pImpl_->zhama_value_.end(), 
        [&value](const std::int32_t val) {
        return value == val;
    });
    if (iter != pImpl_->zhama_value_.end())
    {
        pImpl_->zhama_num_ = value;
    }
    else
    {
        pImpl_->zhama_num_ = 2;
    }
}

void HzMajiangRoom::set_support_7dui(const bool value)
{
    pImpl_->is_support_7dui = value;
}

std::int32_t HzMajiangRoom::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto cmd = packet->GetCmd();
    DLOG(INFO) << "HzMajiangRoom::OnMessage()->cmd:" << cmd << " roomid:=" << scene_id()
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
    PrivateRoom::OnGameStart();

    auto& seats = table_obj()->GetSeats();
    for (auto iter : seats)
    {
        Cards cards;
        assistx2::Stream package(SERVER_BROADCAST_ON_DEAL);
        if (banker_seatno() == iter->seat_no())
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
        package.Write(static_cast<std::int32_t>(card_generator()->count()));
        package.End();

        DCHECK(iter->player() != nullptr);
        iter->player()->SendTo(package);

        iter->set_seat_player_state(Seat::PLAYER_STATUS_PLAYING);

        iter->data()->hand_cards_ = std::make_shared<CardGroup>(cards);

        assistx2::Stream stream(SERVER_BROADCAST_PLAYER_HANDCARD);
        stream.Write(iter->seat_no());
        stream.Write(static_cast<std::int32_t>(iter->data()->hand_cards_->hand_cards().size()));
        stream.End();

        BroadCast(stream, iter->player());
    }

     pImpl_->BroadCastNextPlayer(banker_seatno());

     auto operations = pImpl_->GetOperations(banker_seatno());
     if (operations.size() == 0)
     {
         operations.push_back(CardLogic::PLAY_OPERA);
     }

     pImpl_->NotifyOperation(banker_seatno(),operations);
}

void HzMajiangRoom::OnGameOver(HuType type)
{
    DLOG(INFO) << "OnGameOver: roomid:=" << scene_id()
        << ",type:=" << static_cast<std::int32_t>(type);
    if (type == PrivateRoom::HuType::ZIMOHU)
    {
        pImpl_->ZhaNiao();
        DCHECK(pImpl_->hu_players_.size() > 0u);
        auto hu_seatno = pImpl_->hu_players_[0];
        DCHECK(hu_seatno != Table::INVALID_SEAT);
        auto num = pImpl_->GetZhongNiaoNum(hu_seatno);
        auto sum_score = 2 + num * 2;
        pImpl_->CalculateScore(hu_seatno, sum_score);
        set_banker_seatno( hu_seatno );
    }
    else if (type == PrivateRoom::HuType::QINAGGANGHU)
    {
        pImpl_->ZhaNiao();

        DCHECK(pImpl_->beiqiang_seat_ != Table::INVALID_SEAT);
        auto beiqiang_seat = table_obj()->GetBySeatNo(pImpl_->beiqiang_seat_);
        DCHECK(beiqiang_seat != nullptr);

        auto players = table_obj()->player_count();
        auto score = players - 1;
        DCHECK(pImpl_->hu_players_.size() > 0u);
        for (auto iter : pImpl_->hu_players_)
        {
            DCHECK(iter != Table::INVALID_SEAT);
            auto seat = table_obj()->GetBySeatNo(iter);
            DCHECK(seat != nullptr);
            auto num = pImpl_->GetZhongNiaoNum(iter);
            auto sum_score = 2 * score + 2*num * score;
            seat->data()->game_score_ += sum_score;
            beiqiang_seat->data()->game_score_ -= sum_score;
        } 
        if (pImpl_->hu_players_.size() >= 2u)
        {
            set_banker_seatno( pImpl_->beiqiang_seat_ );
        }
        else
        {
            set_banker_seatno( pImpl_->hu_players_[0] );
        }
    }
    else 
    {
        set_banker_seatno( pImpl_->last_mo_seat_ );
        //do nothing
    }
    
    auto& seats = table_obj()->GetSeats();
    auto players = table_obj()->player_count();
    assistx2::Stream stream(SERVER_BROADCAST_GAME_ACCOUNT);
    stream.Write(static_cast<std::int32_t>(type));
    if (pImpl_->hu_card_ == nullptr)
    {
        DLOG(INFO) << "OnGameOver: roomid:=" << scene_id() << ",hu_card:=" << "";
        stream.Write(std::string(""));
    }
    else
    {
        DLOG(INFO) << "OnGameOver: roomid:=" << scene_id() << ",hu_card:="
            << pImpl_->hu_card_->getName();
        stream.Write(pImpl_->hu_card_->getName());
    }
    stream.Write(static_cast<std::int32_t>(pImpl_->hu_players_.size()));
    for (auto iter : pImpl_->hu_players_)
    {
        DLOG(INFO) << "OnGameOver: roomid:=" << scene_id() << ",hu_seatno:="
            << iter;
        stream.Write(iter);
    }
    stream.Write(players);
    for (auto iter : seats)
    {
        iter->data()->seat_score_ += iter->data()->game_score_;
        stream.Write(iter->seat_no());
        stream.Write(static_cast<std::int32_t>(iter->player()->uid()));
        stream.Write(iter->player()->member_fides()->name());
        stream.Write(iter->data()->game_score_);
        stream.Write(iter->data()->seat_score_);
        DLOG(INFO) << "OnGameOver: roomid:=" << scene_id() << ",seatno:=" << iter->seat_no() << ",mid:="
            << iter->player()->uid() << ",name:=" << iter->player()->member_fides()->name()
            << ",game_score:=" << iter->data()->game_score_ << ",seat_score:="
            << iter->data()->seat_score_;
        //存牌区的牌
        std::stringstream ss;
        auto operated_cards = iter->data()->operated_cards_;
        stream.Write(static_cast<std::int32_t>(operated_cards.size()));
        ss << ",operated_cards-->";
        for (auto it : operated_cards)
        {
            ss << it.first->getName() << ":" << it.second << ",";
            stream.Write(it.first->getName());
            stream.Write(static_cast<std::int32_t>(it.second));
        }
        //手上的牌
        auto hand_cards = iter->data()->hand_cards_->hand_cards();
        stream.Write(static_cast<std::int32_t>(hand_cards.size()));
        ss << ",hand_cards-->";
        for (auto card_iter : hand_cards)
        {
            ss << card_iter->getName() << ",";
            stream.Write(card_iter->getName());
        }
        DLOG(INFO) << "OnGameOver: roomid:=" << scene_id() << ",mid:=" << 
            iter->player()->uid() << ss.str();
    }
    stream.End();

    BroadCast(stream);

    pImpl_->ClearGameData();

    PrivateRoom::OnGameOver(type);
}

void HzMajiangRoom::OnDisbandRoom()
{
    if (room_state() == RoomBase::RoomState::PLAYING)
    {
        OnGameOver(PrivateRoom::HuType::CHOUZHUANG);
    }
    pImpl_->ClearRoomData();

    PrivateRoom::OnDisbandRoom();
}

void HzMajiangRoom::OnReConect(std::shared_ptr<Agent > player)
{
    PrivateRoom::OnReConect(player);

    if (room_state() != RoomBase::RoomState::PLAYING)
    {
        return;
    }

    pImpl_->NotifyTableData(player);
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
    DLOG(INFO) << "BroadCastNextPlayer: roomid:=" << owner_->scene_id()
        << ",seatno:=" << seatno;

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

    seat->data()->now_operate_ = operate;

    assistx2::Stream package(SERVER_NOTIFY_OPERATE);
    package.Write(seatno);
    package.Write(static_cast<std::int32_t>(operate.size()));
    for (auto iter : operate)
    {
        package.Write(static_cast<std::int32_t>(iter));
        DLOG(INFO) << "NotifyOperation: roomid:=" << owner_->scene_id() << 
            ",mid:=" << seat->player()->uid() << ",operate:=" << iter;
    }
    package.End();

    seat->player()->SendTo(package);   
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
        DLOG(ERROR) << "OnPlay:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::PLAY_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }
   
    auto card_name = packet->Read<std::string>();
    auto card = CardFactory::MakeMajiangCard(card_name);
    if (card == nullptr)
    {
        DLOG(ERROR) << "OnPlay:PlayCard Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",card_name:=" << card_name;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }

    DLOG(INFO) << "OnPlay:PlayCard: roomid:=" << owner_->scene_id() 
        << ",mid:=" << player->uid() << ",card_name:=" << card_name;

    auto res = seat->data()->hand_cards_->RemoveCard(card);
    if (res == false)
    {
        DLOG(ERROR) << "OnPlay:PlayCard Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid()<< ",card_name:=" << card_name << "not in hand_cards";
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }
    seat->data()->played_cards_.push(card);
    seat->data()->now_operate_.clear();
    seat->data()->mo_card_ = nullptr;

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
        DLOG(ERROR) << "OnPeng:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::PENG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    DCHECK(now_played_seatno_ != Table::INVALID_SEAT);
    auto played_seat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
    DCHECK(played_seat != nullptr);

    auto card = played_seat->data()->played_cards_.top();
    played_seat->data()->played_cards_.pop();

    now_played_seatno_ = Table::INVALID_SEAT;

    DCHECK(card_logic_->CheckPeng(card, seat->data()->hand_cards_) == true);
    CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
    CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);

    seat->data()->operated_cards_.insert(std::make_pair(card, PENG));
    seat->data()->now_operate_.clear();

    assistx2::Stream stream(SERVER_BROADCAST_PENG_CARD);
    stream.Write(seatno);
    stream.Write(played_seat->seat_no());
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);


    BroadCastNextPlayer(seatno);
    
    NotifyOperation(seatno, std::vector<CardLogic::OperationType>(1,CardLogic::PLAY_OPERA));
}

void HzMajiangRoomImpl::OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::GANG_OPERA))
    {
        DLOG(ERROR) << "OnGang:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::GANG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONGANG, -1);
        return;
    }

    OperCardsType type = NULL_TYPE;
    std::shared_ptr<Card> card = nullptr;
    Seat* operaseat = nullptr;
    if (active_player_ == now_operator_)
    {
        operaseat = seat;
        auto card_name = packet->Read<std::string>();
        auto card_recv = CardFactory::MakeMajiangCard(card_name);
        if (card_recv != nullptr && 
            seat->data()->hand_cards_->card_count(card_recv) == 4)
        {
            type = AGANG;
            card = card_recv;
        }
        else
        {
            auto cards_info = seat->data()->hand_cards_->hand_cards_info();
            for (auto iter : cards_info)
            {
                if (iter.num == 4)
                {
                    type = AGANG;
                    card = iter.card;
                    break;
                }
            }
        }
        if (type == NULL_TYPE)
        {
            for (auto iter : seat->data()->operated_cards_)
            {
                if (iter.second != PENG)
                {
                    continue;
                }
                DCHECK(seat->data()->mo_card_ != nullptr);
                if (iter.first->getFace() == seat->data()->mo_card_->getFace())
                {
                    type = GGANG;
                    card = seat->data()->mo_card_;
                    CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
                    break;
                }
            }
        }
        else
        {
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        }
    }
    else
    {
        DCHECK(now_played_seatno_ != Table::INVALID_SEAT);
        auto played_seat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
        DCHECK(played_seat != nullptr);

        operaseat = played_seat;

        card = played_seat->data()->played_cards_.top();
        played_seat->data()->played_cards_.pop();

        DCHECK(card_logic_->CheckGang(card, seat->data()->hand_cards_) == true);

        now_played_seatno_ = Table::INVALID_SEAT;
       
        played_seat->data()->game_score_ -= 3;
        seat->data()->game_score_ += 3;

        seat->data()->gang_ming_num_ += 1;
        played_seat->data()->gang_fang_num_ += 1;

        type = MGANG;
        
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
    }

    CHECK(card != nullptr);

    seat->data()->now_operate_.clear();

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
        if (res == true)
        {
            seat->data()->beiqiang_hu_num_ += 1;
            return;
        }
        seat->data()->operated_cards_.erase(card);
        seat->data()->gang_gong_num_ += 1;
        CalculateScore(seatno,1);
    }
    else if (type == AGANG)
    {
        seat->data()->gang_an_num_ += 1;
        CalculateScore(seatno, 2);
    }

    seat->data()->operated_cards_.insert(std::make_pair(card, type));
 
    BroadCastNextPlayer(seatno);

    MoCard(player);
}

void HzMajiangRoomImpl::OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    if (active_player_ != now_operator_)
    {
        SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::HUPAI_OPERA))
    {
        DLOG(ERROR) << "OnPeng:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::HUPAI_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    seat->data()->now_operate_.clear();
    hu_card_ = seat->data()->mo_card_;

    seat->data()->zimo_num_ += 1;

    auto& seats = owner_->table_obj()->GetSeats();
    assistx2::Stream stream(SERVER_BROADCAST_HU_CARD);
    stream.Write(seatno);
    stream.Write(owner_->table_obj()->player_count());
    for (auto iter : seats)
    {
        auto hand_cards = iter->data()->hand_cards_->hand_cards();
        stream.Write(iter->seat_no());
        stream.Write(static_cast<std::int32_t>(hand_cards.size()));
        for (auto it : hand_cards)
        {
            stream.Write(it->getName());
        }
    }
    stream.End();

    owner_->BroadCast(stream);

    hu_players_.push_back(seatno);

    owner_->OnGameOver(PrivateRoom::HuType::ZIMOHU);
}
void HzMajiangRoomImpl::OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    if (now_operator_ != seatno)
    {
        DLOG(ERROR) << "OnCancle: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",now_operator_:=" << now_operator_ << " != seatno:=" << seatno;
        SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, -1);
    }

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (!CheckOperation(seat, CardLogic::CANCLE_OPERA))
    {
        DLOG(ERROR) << "OnCancle:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::CANCLE_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, -1);
        return;
    }

    seat->data()->now_operate_.clear();
    SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, 0);

    if (active_player_ == now_operator_)
    {
        NotifyOperation(seatno, std::vector<CardLogic::OperationType>(1, CardLogic::PLAY_OPERA));
    }
    else
    {
        DCHECK(active_player_ != Table::INVALID_SEAT);
        auto nextseat = owner_->table_obj()->next_seat(active_player_);
        DCHECK(nextseat != nullptr);

        BroadCastNextPlayer(nextseat->seat_no());
        MoCard(nextseat->player());
    }
    
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
        return true;
    }

    auto iter = std::find_if(seat->data()->now_operate_.begin(), seat->data()->now_operate_.end(), 
        [&operate](const CardLogic::OperationType value) {
        return value == operate;
    });
    auto res = true;
    if (iter == seat->data()->now_operate_.end())
    {
        res = false;
    }
    return res;
}

std::vector<CardLogic::OperationType> HzMajiangRoomImpl::GetOperations(std::int32_t seatno)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    std::vector<CardLogic::OperationType> operatios;

    auto res = CheckHu(nullptr, seat);
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
    DLOG(INFO) << "FindOperatorPlayer: roomid:=" << owner_->scene_id()
        << ",seatno:=" << seatno << ",card_name:=" << card->getName();

    auto& seats = owner_->table_obj()->GetSeats();
    auto has_operator = false;

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
        if (operatios.size() != 0u)
        {
            has_operator = true;
            NotifyOperation(iter->seat_no(),operatios);
            break;
        }
    }

    if (has_operator == false)
    {
        auto nextseat = owner_->table_obj()->next_seat(seatno);
        BroadCastNextPlayer(nextseat->seat_no());

        MoCard(nextseat->player());
    }
}

bool HzMajiangRoomImpl::FindGongGang(Seat* seat, std::shared_ptr<Card> card)
{
    for (auto iter : seat->data()->operated_cards_)
    {
        if (iter.second != PENG)
        {
            continue;
        }
        if (iter.first->getFace() == card->getFace() &&
            iter.first->getType() == card->getType())
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
        auto res = CheckHu(card, iter);
        if (res == true)
        {
            hu_card_ = card;
            hu_players_.push_back(iter->seat_no());
            auto& seats = owner_->table_obj()->GetSeats();
            assistx2::Stream stream(SERVER_BROADCAST_QIANGGANG_HU);
            stream.Write(iter->seat_no());
            stream.Write(seatno);
            stream.Write(card->getName());
            stream.Write(owner_->table_obj()->player_count());
            for (auto seat_iter : seats)
            {
                auto hand_cards = seat_iter->data()->hand_cards_->hand_cards();
                stream.Write(seat_iter->seat_no());
                stream.Write(static_cast<std::int32_t>(hand_cards.size()));
                for (auto it : hand_cards)
                {
                    stream.Write(it->getName());
                }
            }
            stream.End();

            owner_->BroadCast(stream);
            iter->data()->gang_hu_num_ += 1;
        }
    }

    if (hu_players_.size() > 0u)
    {
        beiqiang_seat_ = seatno;
        owner_->OnGameOver(PrivateRoom::HuType::QINAGGANGHU);
        return true;
    }

    return false;
}

void HzMajiangRoomImpl::MoCard(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto cardcount = static_cast<std::int32_t>(
        owner_->card_generator()->count());
    if (cardcount == zhama_num_)
    {
        return owner_->OnGameOver(
            PrivateRoom::HuType::CHOUZHUANG);
    }

    last_mo_seat_ = seatno;

    auto card = owner_->card_generator()->Pop();
    assistx2::Stream package(SERVER_NOTIFY_MO_CARD);
    package.Write(seatno);
    package.Write(card->getName());
    package.Write(cardcount);
    package.End();

    player->SendTo(package);

    assistx2::Stream stream(SERVER_BROADCAST_PLAYER_MO);
    stream.Write(seatno);
    stream.Write(cardcount);
    stream.End();

    owner_->BroadCast( stream, player);

    DLOG(INFO) << "MoCard: roomid:=" << owner_->scene_id() << ",mid:=" 
        << player->uid() << ",card_name:=" << card->getName();

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

void HzMajiangRoomImpl::CalculateScore(std::int32_t seatno, std::int32_t score)
{
    DLOG(INFO) << "CalculateScore: roomid:=" << owner_->scene_id() 
        << ",seatno:=" << seatno <<",score:=" << score;
    auto& seats = owner_->table_obj()->GetSeats();
    auto players = owner_->table_obj()->player_count();
    for (auto iter : seats)
    {
        if (iter->seat_no() == seatno)
        {
            iter->data()->game_score_ += score*(players - 1);
        }
        else
        {
            iter->data()->game_score_ -= score;
        }
    }
}

void HzMajiangRoomImpl::ZhaNiao()
{
    std::string niao_str;
    std::vector<std::shared_ptr<Card>> niao;
    for (std::int32_t i = 0; i < zhama_num_; ++i)
    {
        auto card = owner_->card_generator()->Pop();
        niao.push_back(card);
        niao_str += card->getName() + ",";
    }

    DLOG(INFO) << "ZhaNiao: roomid:=" << owner_->scene_id() << ",niao:" << niao_str;
    
    for (auto iter_card : niao)
    {
        auto& seats = owner_->table_obj()->GetSeats();
        for (auto iter : seats)
        {
            auto num = iter->seat_no() - owner_->banker_seatno();//玩家和庄家的距离
            if (iter_card->getFace() == Card::Face::HongZ &&
                iter->seat_no() == owner_->banker_seatno())
            {
                //do nothing
            }
            else if (iter_card->getFace() == (Card::Face::One + num) ||
                iter_card->getFace() == (Card::Face::Five + num) ||
                iter_card->getFace() == (Card::Face::Nine + num))
            {
                //do nothing
            }
            else
            {
                continue;
            }

            auto it = zhong_ma_seats_.find(iter->seat_no());
            if (it != zhong_ma_seats_.end())
            {
                it->second.push_back(iter_card);
            }
            else
            {
                zhong_ma_seats_.insert(std::make_pair(iter->seat_no(), std::vector<std::shared_ptr<Card>>(1,iter_card)));
            }
        }
    }
    std::stringstream ss;
    assistx2::Stream stream(SERVER_BROADCAST_ZHA_NIAO);
    stream.Write(static_cast<std::int32_t>(zhong_ma_seats_.size()));
    for (auto iter : zhong_ma_seats_)
    {
        ss << ",zhongniao seatno:=" << iter.first << ",card:=";
        stream.Write(iter.first);
        stream.Write(static_cast<std::int32_t>(iter.second.size()));
        for (auto it : iter.second)
        {
            ss << it->getName() + ",";
            stream.Write(it->getName());
        }
    }
    stream.End();
    owner_->BroadCast(stream);

    DLOG(INFO) << "ZhaNiao: roomid:=" << owner_->scene_id() << ss.str();
}

std::int32_t HzMajiangRoomImpl::GetZhongNiaoNum(std::int32_t seatno)
{
    auto it = zhong_ma_seats_.find(seatno);
    if (it != zhong_ma_seats_.end())
    {
        return it->second.size();
    }

    return 0;
}

void HzMajiangRoomImpl::ClearGameData()
{
    active_player_ = Table::INVALID_SEAT;
    now_operator_ = Table::INVALID_SEAT;
    now_played_seatno_ = Table::INVALID_SEAT;
    last_mo_seat_ = Table::INVALID_SEAT;
    beiqiang_seat_ = Table::INVALID_SEAT;
    hu_players_.clear();
    zhong_ma_seats_.clear();
    hu_card_ = nullptr;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->ClearNowGameData();
        iter->set_seat_player_state(Seat::PLAYER_STATUS_WAITING);
    }
}

void HzMajiangRoomImpl::ClearRoomData()
{
    active_player_ = Table::INVALID_SEAT;
    now_operator_ = Table::INVALID_SEAT;
    now_played_seatno_ = Table::INVALID_SEAT;
    last_mo_seat_ = Table::INVALID_SEAT;
    beiqiang_seat_ = Table::INVALID_SEAT;
    zhama_num_ = 2;
    is_support_7dui = false;
    hu_players_.clear();
    zhong_ma_seats_.clear();
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->ClearAllData();
        iter->set_seat_player_state(Seat::PLAYER_STATUS_WAITING);
    }
}

void HzMajiangRoomImpl::NotifyTableData(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);
    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    auto mo_card = seat->data()->mo_card_;
    auto hand_cards = seat->data()->hand_cards_->hand_cards();
    assistx2::Stream stream(SERVER_NOTIFY_PLAYER_TABLE_DATA);
    if (mo_card != nullptr)
    {
        stream.Write(mo_card->getName());
    }
    else
    {
        stream.Write(std::string(""));
    }
    //手上的牌
    stream.Write(static_cast<std::int32_t>(hand_cards.size()));
    for (auto iter : hand_cards)
    {
        stream.Write(iter->getName());
    }
    auto& seats = owner_->table_obj()->GetSeats();
    stream.Write(static_cast<std::int32_t>(
        owner_->table_obj()->player_count()));
    for (auto iter : seats)
    {
        stream.Write(iter->seat_no());
        auto seat_hand_cards = iter->data()->hand_cards_->hand_cards();
        stream.Write(static_cast<std::int32_t>(seat_hand_cards.size()));
        //存牌区的牌
        auto operated_cards = iter->data()->operated_cards_;
        stream.Write(static_cast<std::int32_t>(operated_cards.size()));
        for (auto it : operated_cards)
        {
            stream.Write(it.first->getName());
            stream.Write(static_cast<std::int32_t>(it.second));
        }
        //出牌区的牌
        auto played_cards = iter->data()->played_cards_;
        stream.Write(static_cast<std::int32_t>(played_cards.size()));
        while (!played_cards.empty())
        {
            auto card = played_cards.top();
            played_cards.pop();
            stream.Write(card->getName());
        }
    }
    stream.Write(now_played_seatno_);
    stream.Write(active_player_);
    stream.Write(static_cast<std::int32_t>(
        owner_->card_generator()->count()));
    stream.End();

    player->SendTo(stream);

    if (now_operator_ == seatno)
    {
        NotifyOperation(seatno, seat->data()->now_operate_);
    }
}

bool HzMajiangRoomImpl::CheckHu(const std::shared_ptr<Card> card, Seat* seat)
{
    auto res = card_logic_->CheckHu(card, seat->data()->hand_cards_);
    if (is_support_7dui == true && res == false)
    {
        if (seat->data()->operated_cards_.size() != 0u)
        {
            return res;
        }
        res = card_logic_->CheckHu7Dui(card, seat->data()->hand_cards_);
    }
    
    return res;
}