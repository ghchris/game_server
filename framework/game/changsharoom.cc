#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include <json_spirit_writer_template.h>
#include <assistx2/json_wrapper.h>
#include <functional>
#include "changsharoom.h"
#include "table.h"
#include "seat.h"
#include "cardgenerator.h"
#include "seatdata.h"
#include "csmajianglogic.h"
#include "agent.h"
#include "cardgroup.h"
#include "gameconfigdata.h"
#include "scenetimer.h"
#include "memberfides.pb.h"
#include "gamedatamanager.h"
#include "logserver.h"
#include "common.h"
#include "zzmajianglogic_twoplayer.h"

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
const static std::int16_t SERVER_BROADCAST_ROOM_ACCOUNT = 1035;//服务器广播总结算
const static std::int16_t CLIENT_REQUEST_ONCHI = 1036;//用户吃
const static std::int16_t SERVER_BROADCAST_CHI_CARD = 1036;//广播吃
const static std::int16_t CLIENT_REQUEST_ONBU = 1037;//用户补张
const static std::int16_t SERVER_BROADCAST_BU_CARD = 1037;//广播补张
const static std::int16_t SERVER_NOTIFY_BU_OPERATION = 1038;//服务器通知杠后补张的操作
const static std::int16_t CLIENT_REQUEST_BU_OPERATION = 1039;
const static std::int16_t SERVER_RESPONSE_BU_OPERATION = 1039;
const static std::int16_t SERVER_NOTIFY_PLAY_BUCARD = 1040;
const static std::int16_t SERVER_NOTIFY_ISTING = 1041;//服务器通知是否听牌
const static std::int16_t SERVER_NOTIFY_MT_OPERATION = 1042;//服务器通知起手明堂操作
const static std::int16_t CLIENT_REQUEST_MINGTANG_OPERATION = 1043;
const static std::int16_t SERVER_RESPONSE_MINGTANG_OPERATION = 1043;
const static std::int16_t SERVER_NOTIFY_HAIDIMANYOU = 1044;//服务器通知海底漫游
const static std::int16_t CLIENT_REQUEST_HAIDIMANYOU = 1044;//用户请求海底漫游
const static std::int16_t SERVER_BROADCAST_HAIDI_CARD = 1045;//服务器广播海底牌
const static std::int16_t SERVER_BROADCAST_HAIDI_HU = 1046;//海底胡
const static std::int16_t SERVER_BROADCAST_HAIDI_PAO = 1047;//海底胡
const static std::int16_t SERVER_BROADCAST_GANG_HU = 1048;//刚上花
const static std::int16_t SERVER_BROADCAST_GANG_PAO = 1049;//刚上炮
const static std::int16_t SERVER_BROADCAST_MINGTANG = 1050;

class ChangShaRoomImpl
{
public:
    ChangShaRoomImpl(ChangShaRoom* owner);
    ~ChangShaRoomImpl();
    void BroadCastNextPlayer(std::int32_t seatno);
    void NotifyOperation(std::int32_t seatno,
        std::map<CardLogic::OperationType, OperationsInfo> operate);
    void NotifyBuOperation(std::int32_t seatno);
    void NotifyMTOperation(std::int32_t seatno, std::set<MingTangOperationType> operate);
    bool CheckOperation(Seat* seat, CardLogic::OperationType operate, std::function<void(Seat*, std::string)>& func);
    void SendErrorCode(std::shared_ptr<Agent > player, const std::int16_t cmd, const std::int32_t err);
    std::map<CardLogic::OperationType, OperationsInfo> GetOperations(std::int32_t seatno,bool checkhu = true);
    std::map<CardLogic::OperationType, OperationsInfo> GetOperations(std::int32_t seatno,std::shared_ptr<Card> card);
    void BroadCastPlayedCard(std::int32_t seatno, std::shared_ptr<Card> card);
    void FindOperatorPlayer(std::int32_t seatno, std::shared_ptr<Card> card);
    bool FindGongGang(Seat* seat, std::shared_ptr<Card> card);
    bool FindQiangGangHu(std::int32_t seatno, std::shared_ptr<Card> card);
    void MoCard(std::shared_ptr<Agent > player);
    void BuCard(std::shared_ptr<Agent > player);
    void CalculateScore(std::int32_t seatno, std::int32_t score);
    void ZhaNiao();
    bool CheckHu(const std::shared_ptr<Card> card,Seat* seat);
    bool CheckBuZhang(const std::shared_ptr<Card> card, Seat* seat);
    bool CheckCanGang(const std::shared_ptr<Card> card, Seat* seat);
    void IncrMingtang(Seat* seat,MingTangType type);
    std::int32_t GetZhongNiaoNum(std::int32_t seatno);
    void ClearGameData();
    void ClearRoomData();
    void NotifyTableData(std::shared_ptr<Agent > player);
    void RoomAccount();
    bool IsZhuangXianCalculateScore();
    void DoPlay(Seat* seat, std::string card);
    void DoChi(Seat* seat, std::string card);
    void DoPeng(Seat* seat, std::string card);
    void DoGang(Seat* seat, std::string card);
    void DoZiMoHu(Seat* seat, std::string card);
    void DoDianPaoHu(Seat* seat, std::string card);
    void DoGongGang(Seat* seat, std::string card);
    void DoBuZhang(Seat* seat, std::string card);
    void DoHaidiHu(Seat* seat, std::string card);
    void DoHaidiPao(Seat* seat, std::string card);
    void DoGangHu(Seat* seat, std::string card);
    void DoGangPao(Seat* seat, std::string card);
    void AddZhongMaSeats(const std::int32_t seatno,const std::shared_ptr<Card> card);
    bool IsNoOperator();
    void ClearOperation(CardLogic::OperationType operate);
    bool IsQingYiSe(std::shared_ptr<Card>card, Seat* seat);
    bool IsPengPeng(std::shared_ptr<Card>card, Seat* seat);
    bool IsJiangJiang(std::shared_ptr<Card>card, Seat* seat);
    std::vector< std::shared_ptr<Card>> GetGangCard(Seat* seat);
    void NotifyBuZhang(std::shared_ptr<Agent > player);//杠后系统补两张牌
    void NotifyMoCard(std::shared_ptr<Agent > player);
    std::map<CardLogic::OperationType, OperationsInfo>
        GetOtherPlayerOperations(std::int32_t seatno, std::shared_ptr<Card> card);
    bool IsNoGangOperator();
    std::function<void(Seat*, std::string)> GetOperatorFunc(
        CardLogic::OperationType operate, Seat* seat, std::string card);
    void NotifyPlayBuCard(Seat* seat,std::shared_ptr<Card> card1, std::shared_ptr<Card> card2);
    void RemoveBuCard(Seat* seat, std::shared_ptr<Card> card);
    bool CheckCanOperator(Seat* seat, CardLogic::OperationType operate);
    std::int32_t GetOperationLevel(CardLogic::OperationType operate);
    void ChangeBuCard(Seat* seat, std::shared_ptr<Card> card); 
    std::set<MingTangOperationType> GetQSMTOperation(Seat* seat);//获取起手明堂
    void ClearMingtangOperation(Seat* seat,MingTangOperationType operation);
    void NotifyHaidiManYou(std::int32_t seatno);
    void NotifyHaidiCard();
    std::vector<MingTangType> SetMingTang(std::shared_ptr<Card> card, Seat* seat);
    void NotifyMingTangType(std::int32_t seatno,std::vector<MingTangType>& mingtang);
    void CalculateMingTang(Seat* seat, MingTangOperationType operation);
    void SetGuoCard(Seat* seat);
    std::shared_ptr<Card> GetGGangCard(Seat* seat,std::shared_ptr<Card> card);
public:
    void OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnChi(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnBuZhang(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnGangBuOperation(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnMintangOperation(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnHaidiManYou(std::shared_ptr<Agent > player, assistx2::Stream * packet);
public:
    ChangShaRoom* owner_;
    std::int32_t active_player_ = Table::INVALID_SEAT;//当前激活的玩家座位号(房间里的玩家都能看见)
    std::int32_t now_played_seatno_ = Table::INVALID_SEAT;//当前打出牌的座位号
    std::int32_t last_mo_seat_ = Table::INVALID_SEAT;//最后摸牌的座位号
    std::int32_t beiqiang_seat_ = Table::INVALID_SEAT;//被抢杠的座位号
    std::shared_ptr<CardLogic> card_logic_;
    std::vector<std::int32_t> hu_players_;
    std::int32_t zhama_num_ = 0;//扎码的个数 默认值0
    std::map<std::int32_t, std::vector<std::shared_ptr<Card>>> zhong_ma_seats_;
    std::shared_ptr<Card> hu_card_ = nullptr;//胡的牌
    std::int32_t room_operation_ = 0;//房间选项 1为庄闲
    std::vector<std::int32_t> zhama_value_{ 0,2,3,4,6,8 };
    std::vector<std::shared_ptr<Card>> vec_niao_;
    PrivateRoom::HuType hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    time_t now_active_time_ = 0;
    std::function<void(Seat*, std::string)> now_operation_func_ = nullptr;
    std::int32_t zhama_start_seatno_ = Table::INVALID_SEAT;//计算扎码的起始位置
    std::vector<assistx2::Stream> hu_message_;
    std::int32_t gang_seatno_ = Table::INVALID_SEAT;
    std::int32_t bu_card_operator_seatno = Table::INVALID_SEAT;//操作补张牌的玩家座位号
    CardLogic::OperationType bu_card_operation_ = CardLogic::NULL_OPERA;
    std::int32_t manyou_start_seatno_ = Table::INVALID_SEAT;
    std::int32_t now_manyou_seatno_ = Table::INVALID_SEAT;
    std::int32_t haidicard_owner_seatno_ = Table::INVALID_SEAT;
};

ChangShaRoom::ChangShaRoom(std::uint32_t id, std::string type):
 PrivateRoom(id,type),
 pImpl_(new ChangShaRoomImpl(this))
{
    pImpl_->card_logic_ = std::make_shared<CSMajiangLogic>();
}

ChangShaRoom::~ChangShaRoom()
{
    
}

void ChangShaRoom::set_zhama_num(const std::int32_t value)
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
        pImpl_->zhama_num_ = 0;
    }
}

void ChangShaRoom::set_operation(const std::int32_t value)
{
    pImpl_->room_operation_ = value;
}

std::int32_t ChangShaRoom::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto cmd = packet->GetCmd();
    DLOG(INFO) << "ChangShaRoom::OnMessage()->cmd:" << cmd << " roomid:=" << scene_id()
        << " Scene Type:=" << scene_type() << " mid:=" << player->uid();

    switch (cmd)
    {
    case CLIENT_REQUEST_ONPLAY:
        pImpl_->OnPlay(player, packet);
        return 0;
    case CLIENT_REQUEST_ONCHI:
        pImpl_->OnChi(player, packet);
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
    case CLIENT_REQUEST_ONBU:
        pImpl_->OnBuZhang(player, packet);
        return 0;
    case  CLIENT_REQUEST_BU_OPERATION:
        pImpl_->OnGangBuOperation(player, packet);
        return 0;
    case CLIENT_REQUEST_MINGTANG_OPERATION:
        pImpl_->OnMintangOperation(player, packet);
        return 0;
    case CLIENT_REQUEST_HAIDIMANYOU:
        pImpl_->OnHaidiManYou(player, packet);
        return 0;
    default:
        return PrivateRoom::OnMessage(player,packet);
    }

    return 0;
}

void ChangShaRoom::OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context)
{
    PrivateRoom::OnTimer(context);

    if (room_state() != RoomBase::RoomState::PLAYING)
    {
        return;
    }

    auto ptr = dynamic_cast<EventTimerContext *>(context.get());
    switch (ptr->type_)
    {
    case SceneTimerContext::MAJIANG_MOPAI:
        pImpl_->MoCard(ptr->seat_->player());
        break;
    case SceneTimerContext::MAJIANG_BUPAI:
        pImpl_->BuCard(ptr->seat_->player());
        break;
    case SceneTimerContext::MAJIANG_GANG_BUPAI:
        pImpl_->NotifyBuZhang(ptr->seat_->player());
        break;
    case SceneTimerContext::MAJIANG_GAMEOVER:
        OnGameOver();
        break;
    default:
        break;
    }
}

void ChangShaRoom::OnGameStart()
{
    PrivateRoom::OnGameStart();

    card_generator()->Reset(CardGenerator::Type::GENERAL_MAJIANG);

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

        auto state = ((iter->seat_player_state() & Seat::PLAYER_STATUS_NET_CLOSE) | Seat::PLAYER_STATUS_PLAYING);
        iter->set_seat_player_state(state);

        iter->data()->hand_cards_ = std::make_shared<CardGroup>(cards);
        iter->data()->start_playing_time_ = time(nullptr);

        assistx2::Stream stream(SERVER_BROADCAST_PLAYER_HANDCARD);
        stream.Write(iter->seat_no());
        stream.Write(static_cast<std::int32_t>(iter->data()->hand_cards_->hand_cards().size()));
        stream.Write(static_cast<std::int32_t>(card_generator()->count()));
        stream.End();

        BroadCast(stream, iter->player());
    }
    GameDataManager::getInstance()->OnGameStart(this);

    pImpl_->BroadCastNextPlayer(banker_seatno());

    auto is_have_mt = false;
    for (auto iter : seats)
    {
        auto mtoperations = pImpl_->GetQSMTOperation(iter);
        if (mtoperations.size() != 0)
        {
            is_have_mt = true;
            pImpl_->NotifyMTOperation(iter->seat_no(), mtoperations);
        }
    }

    if (is_have_mt == false)
    {
        auto operations = pImpl_->GetOperations(banker_seatno());
        if (operations.size() == 0)
        {
            operations.emplace(CardLogic::PLAY_OPERA, OperationsInfo());
        }

        pImpl_->NotifyOperation(banker_seatno(), operations);
    }
}

void ChangShaRoom::OnGameOver()
{
    DLOG(INFO) << "OnGameOver: roomid:=" << scene_id()
        << ",type:=" << static_cast<std::int32_t>(pImpl_->hu_type_);

    auto now_banker_seatno = banker_seatno();
    DCHECK(now_banker_seatno != Table::INVALID_SEAT);

    if (pImpl_->hu_type_ == PrivateRoom::HuType::ZIMOHU ||
        pImpl_->hu_type_ == PrivateRoom::HuType::GANGHU ||
        pImpl_->hu_type_ == PrivateRoom::HuType::HAIDIHU)
    {
        pImpl_->ZhaNiao();
        DCHECK(pImpl_->hu_players_.size() > 0u);
        auto hu_seatno = pImpl_->hu_players_[0];
        DCHECK(hu_seatno != Table::INVALID_SEAT);
        auto hu_seat = table_obj()->GetBySeatNo(hu_seatno);
        auto sum_score = pImpl_->GetZhongNiaoNum(hu_seatno);
        pImpl_->CalculateScore(hu_seatno, sum_score);
        for (auto iter : table_obj()->GetSeats())
        {
            if (iter->seat_no() == hu_seatno)
            {
                continue;
            }
            auto num = pImpl_->GetZhongNiaoNum(iter->seat_no());
            hu_seat->data()->game_score_ += num;
            iter->data()->game_score_ -= num;
        }
        if (pImpl_->IsZhuangXianCalculateScore())
        {
            if (hu_seatno == now_banker_seatno)
            {
                pImpl_->CalculateScore(hu_seatno, 1);
            }
            else
            {
                auto now_banker_seat = table_obj()->GetBySeatNo(now_banker_seatno);
                now_banker_seat->data()->game_score_ -= 1;
                hu_seat->data()->game_score_ += 1;
            }
        }
        set_banker_seatno( hu_seatno );
    }
    else if (pImpl_->hu_type_ == PrivateRoom::HuType::QINAGGANGHU ||
        pImpl_->hu_type_ == PrivateRoom::HuType::DIANPAOHU ||
        pImpl_->hu_type_ == PrivateRoom::HuType::GANGPAO ||
        pImpl_->hu_type_ == PrivateRoom::HuType::HAIDIPAO)
    {
        DCHECK(pImpl_->beiqiang_seat_ != Table::INVALID_SEAT);
        auto beiqiang_seat = table_obj()->GetBySeatNo(pImpl_->beiqiang_seat_);
        DCHECK(beiqiang_seat != nullptr);
        DCHECK(pImpl_->hu_players_.size() > 0u);
        if (pImpl_->hu_players_.size() > 1)
        {
            pImpl_->zhama_start_seatno_ = pImpl_->beiqiang_seat_;
        }
        else
        {
            pImpl_->zhama_start_seatno_ = pImpl_->hu_players_[0];
        }
        pImpl_->ZhaNiao();

        for (auto iter : pImpl_->hu_players_)
        {
            DCHECK(iter != Table::INVALID_SEAT);
            auto seat = table_obj()->GetBySeatNo(iter);
            DCHECK(seat != nullptr);
            auto sum_score = pImpl_->GetZhongNiaoNum(iter);
            seat->data()->game_score_ += sum_score;
            beiqiang_seat->data()->game_score_ -= sum_score;
            if (pImpl_->IsZhuangXianCalculateScore())
            {
                if (iter == now_banker_seatno || 
                    pImpl_->beiqiang_seat_ == now_banker_seatno)
                {
                    seat->data()->game_score_ += 1;
                    beiqiang_seat->data()->game_score_ -= 1;
                }
            }
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
    }
    
    auto& seats = table_obj()->GetSeats();
    auto players = table_obj()->player_count();
    assistx2::Stream stream(SERVER_BROADCAST_GAME_ACCOUNT);
    stream.Write(now_banker_seatno);
    stream.Write(banker_seatno());
    stream.Write(room_conifg_data()->ju - played_num());
    stream.Write(static_cast<std::int32_t>(pImpl_->hu_type_));
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
        iter->data()->end_playing_time_ = time(nullptr);
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
        auto& operated_cards = iter->data()->operated_cards_;
        stream.Write(static_cast<std::int32_t>(operated_cards.size()));
        ss << ",operated_cards-->";
        for (auto it : operated_cards)
        {
            ss << it.first->getName() << ":" << it.second << ",";
            stream.Write(it.first->getName());
            stream.Write(static_cast<std::int32_t>(it.second));
        }
        //手上的牌
        auto& hand_cards = iter->data()->hand_cards_->hand_cards();
        stream.Write(static_cast<std::int32_t>(hand_cards.size()));
        ss << ",hand_cards-->";
        for (auto card_iter : hand_cards)
        {
            ss << card_iter->getName() << ",";
            stream.Write(card_iter->getName());
        }
        DLOG(INFO) << "OnGameOver: roomid:=" << scene_id() << ",mid:=" << 
            iter->player()->uid() << ss.str();
        auto& mingtang = iter->data()->mingtang_types_;
        stream.Write(static_cast<std::int32_t>(mingtang.size()));
        for (auto mingtang_iter : mingtang)
        {
            stream.Write(static_cast<std::int32_t>(mingtang_iter.first));
            stream.Write(mingtang_iter.second);
        }
    }
    stream.Write(static_cast<std::int32_t>(pImpl_->vec_niao_.size()));
    for (auto iter : pImpl_->vec_niao_)
    {
        stream.Write(iter->getName());
    }
    stream.Write(RoomDataToString(false));
    stream.End();

    BroadCast(stream,nullptr,true);

    DLOG(INFO) << "OnGameOver: roomid:=" << scene_id() << ",package size:=" << stream.GetNativeStream().GetSize();
    GameDataManager::getInstance()->OnGameOver(this);
    GameDataManager::getInstance()->OnZhaMa(this, pImpl_->vec_niao_);
    LogServer::getInstance()->WriteSubGameLog(this, played_num(), pImpl_->hu_players_);

    pImpl_->ClearGameData();

    PrivateRoom::OnGameOver();
}

std::int32_t ChangShaRoom::Disband()
{
    pImpl_->ClearRoomData();

    return PrivateRoom::Disband();
}

void ChangShaRoom::OnDisbandRoom(DisbandType type)
{
    auto played_num_tmp = played_num();
    if (room_state() == RoomBase::RoomState::PLAYING)
    {
        pImpl_->hu_type_ = PrivateRoom::HuType::NOTPLAYED;
        OnGameOver();
        set_played_num(played_num_tmp - 1);
    }
    pImpl_->RoomAccount();

    LogServer::getInstance()->WriteGameLog(this, played_num_tmp);

    pImpl_->ClearRoomData();

    PrivateRoom::OnDisbandRoom(type);
}

void ChangShaRoom::OnReConect(std::shared_ptr<Agent > player)
{
    //顺序固定,不能改变
    auto state = room_state();

    PrivateRoom::OnReConect(player);

    if (state != RoomBase::RoomState::PLAYING)
    {
        return;
    }

    pImpl_->NotifyTableData(player);
}

std::string ChangShaRoom::RoomDataToString(bool isContainTableData)
{
    json_spirit::Object root; 
    root.push_back(json_spirit::Pair("room", static_cast<std::int32_t>(scene_id())));
    root.push_back(json_spirit::Pair("roomowner", static_cast<std::int32_t>(room_owner())));
    root.push_back(json_spirit::Pair("banker", banker_seatno()));
    root.push_back(json_spirit::Pair("playednum", played_num()));
    root.push_back(json_spirit::Pair("createnum", create_time()));
    root.push_back(json_spirit::Pair("type", room_conifg_data()->type));
    root.push_back(json_spirit::Pair("ju", room_conifg_data()->ju));
    root.push_back(json_spirit::Pair("players", static_cast<std::int32_t>(table_obj()->GetSeats().size())));
    root.push_back(json_spirit::Pair("playtype", 0));
    root.push_back(json_spirit::Pair("operation", pImpl_->room_operation_));
    root.push_back(json_spirit::Pair("zhama", pImpl_->zhama_num_));
    root.push_back(json_spirit::Pair("proxyuid", static_cast<std::int32_t>(proxy_uid())));

    if (isContainTableData == true)
    {
        json_spirit::Array array;
        auto& seats = table_obj()->GetSeats();
        for (auto iter : seats)
        {
            if (iter->player() == nullptr)
            {
                continue;
            }
            if (iter->player()->agent_type() == 
                Agent::AgentType::ROBOT)
            {
                continue;
            }
            json_spirit::Object seat_json;
            seat_json.push_back(json_spirit::Pair("mid", static_cast<std::int32_t>(iter->player()->uid())));
            seat_json.push_back(json_spirit::Pair("seatno",iter->seat_no()));
            auto value = Common::StringToJson(iter->data()->cs_seat_string_data());
            seat_json.push_back(json_spirit::Pair("data", value));
            array.push_back(seat_json);
        }

        root.push_back(json_spirit::Pair("seats", array));
    }

    return json_spirit::write_string(json_spirit::Value(root));
}

void ChangShaRoom::StringToRoomData(const std::string& str)
{
    auto value = Common::StringToJson(str);
    auto & obj = value.get_obj();
    for (auto iter : obj)
    {
        if (iter.name_ == "operation")
        {
            pImpl_->room_operation_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "zhama")
        {
            pImpl_->zhama_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
    }

    PrivateRoom::StringToRoomData(str);
}

ChangShaRoomImpl::ChangShaRoomImpl(ChangShaRoom* owner) :
    owner_(owner)
{

}

ChangShaRoomImpl::~ChangShaRoomImpl()
{
}

void ChangShaRoomImpl::BroadCastNextPlayer(std::int32_t seatno)
{
    DLOG(INFO) << "BroadCastNextPlayer: roomid:=" << owner_->scene_id()
        << ",seatno:=" << seatno;

    active_player_ = seatno;
    now_active_time_ = time(nullptr);

    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);
    seat->data()->guo_cards_.clear();

    assistx2::Stream package(SERVER_BROADCAST_NEXT_PLAYER);
    package.Write(seatno);
    package.End();

    owner_->BroadCast(package);

    assistx2::Stream stream(SERVER_NOTIFY_ISTING);
    stream.Write(static_cast<std::int32_t>(seat->data()->is_ting));
    stream.End();
    seat->player()->SendTo(stream);
}

void ChangShaRoomImpl::NotifyOperation(std::int32_t seatno,
    std::map<CardLogic::OperationType, OperationsInfo> operate)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);
    DCHECK(seat->player() != nullptr);

    seat->data()->cs_now_operate_ = operate;
    std::stringstream ss;
    ss << seat->player()->uid() << "_" << owner_->scene_id() << "_" << (rand() % 100000 + 1);
    seat->data()->operation_id_ = ss.str();

    assistx2::Stream package(SERVER_NOTIFY_OPERATE);
    package.Write(seatno);
    package.Write(static_cast<std::int32_t>(operate.size()));
    for (auto iter : operate)
    {
        package.Write(static_cast<std::int32_t>(iter.first));
        DLOG(INFO) << "NotifyOperation: roomid:=" << owner_->scene_id() << 
            ",mid:=" << seat->player()->uid() << ",operate:=" << iter.first;
    }
    package.Write(seat->data()->operation_id_);
    package.End();

    seat->player()->SendTo(package);
}

void ChangShaRoomImpl::NotifyMTOperation(std::int32_t seatno, std::set<MingTangOperationType> operate)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);
    DCHECK(seat->player() != nullptr);

    seat->data()->now_mingtang_operate_ = operate;
    std::stringstream ss;
    ss << seat->player()->uid() << "_" << owner_->scene_id() << "_" << (rand() % 100000 + 1);
    seat->data()->operation_id_ = ss.str();

    assistx2::Stream package(SERVER_NOTIFY_MT_OPERATION);
    package.Write(seatno);
    package.Write(static_cast<std::int32_t>(operate.size()));
    for (auto iter : operate)
    {
        package.Write(static_cast<std::int32_t>(iter));
        DLOG(INFO) << "NotifyMTOperation: roomid:=" << owner_->scene_id() <<
            ",mid:=" << seat->player()->uid() << ",operate:=" << iter;
    }
    package.Write(seat->data()->operation_id_);
    package.End();

    seat->player()->SendTo(package);
}

void ChangShaRoomImpl::OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto card_name = packet->Read<std::string>();
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnPlay: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }

    if (!CheckOperation(seat, CardLogic::PLAY_OPERA, now_operation_func_))
    {
        LOG(ERROR) << "OnPlay:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::PLAY_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }
   
    seat->data()->cs_now_operate_.clear();
    auto card = CardFactory::MakeMajiangCard(card_name);
    if (card == nullptr)
    {
        LOG(ERROR) << "OnPlay:PlayCard Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",card_name:=" << card_name;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }

    DoPlay(seat, card_name);
}

void ChangShaRoomImpl::OnChi(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto card_name = packet->Read<std::string>();
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnChi: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONCHI, -1);
        return;
    }

    if (!CheckOperation(seat, CardLogic::CHI_OPERA, now_operation_func_))
    {
        LOG(ERROR) << "OnChi:CheckOperation Failed roomid:=" << owner_->scene_id()
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::CHI_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONCHI, -1);
        return;
    }

    if (now_operation_func_ == nullptr)
    {
        LOG(ERROR) << "OnChi:now_operation_func_ is nullptr roomid:=" << owner_->scene_id()
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::CHI_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONCHI, -1);
        return;
    }

    seat->data()->cs_now_operate_.clear();

    if (IsNoOperator() == true)
    {
        DoChi(seat, card_name);
    }
    else
    {
        now_operation_func_ = std::bind(&ChangShaRoomImpl::DoChi, this, seat, card_name);
        SendErrorCode(player, CLIENT_REQUEST_ONCHI, -2);
    }
}

void ChangShaRoomImpl::OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnPeng: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    if (!CheckOperation(seat, CardLogic::PENG_OPERA, now_operation_func_))
    {
        LOG(ERROR) << "OnPeng:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::PENG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    if (now_operation_func_ == nullptr)
    {
        LOG(ERROR) << "OnPeng:now_operation_func_ is nullptr roomid:=" << owner_->scene_id()
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::PENG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -1);
        return;
    }

    seat->data()->cs_now_operate_.clear();

    ClearOperation(CardLogic::CHI_OPERA);
    
    if (IsNoOperator() == true)
    {
        DoPeng(seat, std::string(""));
    }
    else
    {
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -2);
    }
}

void ChangShaRoomImpl::OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto card_name = packet->Read<std::string>();
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnGang: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONGANG, -1);
        return;
    }

    if (!CheckOperation(seat, CardLogic::GANG_OPERA, now_operation_func_))
    {
        LOG(ERROR) << "OnGang:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::GANG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONGANG, -1);
        return;
    }

    if (now_operation_func_ == nullptr)
    {
        LOG(ERROR) << "OnGang:now_operation_func_ is nullptr roomid:=" << owner_->scene_id()
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::GANG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONGANG, -1);
        return;
    }

    seat->data()->cs_now_operate_.clear();

    ClearOperation(CardLogic::CHI_OPERA);

    if (IsNoOperator() == true)
    {
        DoGang(seat, card_name);
    }
    else
    {
        now_operation_func_ = std::bind(&ChangShaRoomImpl::DoGang, this, seat, card_name);
        SendErrorCode(player, CLIENT_REQUEST_ONGANG, -2);
    }
}

void ChangShaRoomImpl::OnBuZhang(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto card_name = packet->Read<std::string>();
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnBuZhang: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, SERVER_BROADCAST_BU_CARD, -1);
        return;
    }

    if (!CheckOperation(seat, CardLogic::BU_OPERA, now_operation_func_))
    {
        LOG(ERROR) << "OnBuZhang:CheckOperation Failed roomid:=" << owner_->scene_id()
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::BU_OPERA;
        SendErrorCode(player, SERVER_BROADCAST_BU_CARD, -1);
        return;
    }

    if (now_operation_func_ == nullptr)
    {
        LOG(ERROR) << "OnBuZhang:now_operation_func_ is nullptr roomid:=" << owner_->scene_id()
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::BU_OPERA;
        SendErrorCode(player, SERVER_BROADCAST_BU_CARD, -1);
        return;
    }

    seat->data()->cs_now_operate_.clear();

    ClearOperation(CardLogic::CHI_OPERA);

    if (IsNoOperator() == true)
    {
        DoBuZhang(seat, card_name);
    }
    else
    {
        now_operation_func_ = std::bind(&ChangShaRoomImpl::DoBuZhang, this, seat, card_name);
        SendErrorCode(player, SERVER_BROADCAST_BU_CARD, -2);
    }
}

void ChangShaRoomImpl::OnGangBuOperation(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto operation = packet->Read<std::int32_t>();
    auto card_name = packet->Read<std::string>();
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnGangBuOperation: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    if (static_cast<CardLogic::OperationType>(operation) != CardLogic::CANCLE_OPERA)
    {
        auto card = CardFactory::MakeMajiangCard(card_name);
        if (card == nullptr)
        {
            LOG(ERROR) << "OnGangBuOperation: card == nullptr card_name:=" << card_name
                << "roomid: = " << owner_->scene_id() << ",mid:=" << seat->player()->uid();

            SendErrorCode(seat->player(), SERVER_RESPONSE_BU_OPERATION, -1);
            return;
        }

        auto & card_operates = seat->data()->card_now_operates_;
        auto iter = std::find_if(card_operates.begin(), card_operates.end(),
            [&card](std::pair<std::shared_ptr<Card>, std::map<CardLogic::OperationType, OperationsInfo>> value) {
            return card->getName() == value.first->getName();
        });
        if (iter == card_operates.end())
        {
            LOG(ERROR) << "OnGangBuOperation: no such card in operation card_name:=" << card_name
                << "roomid: = " << owner_->scene_id() << ",mid:=" << seat->player()->uid();

            SendErrorCode(seat->player(), SERVER_RESPONSE_BU_OPERATION, -1);
            return;
        }

        seat->data()->cs_now_operate_ = iter->second;
        std::function<void(Seat*, std::string)> func;
        if (!CheckOperation(seat, static_cast<CardLogic::OperationType>(operation), func))
        {
            LOG(ERROR) << "OnGangBuOperation:CheckOperation Failed roomid:=" << owner_->scene_id()
                << ",mid:=" << player->uid() << ",operate:=" << operation;
            SendErrorCode(player, SERVER_RESPONSE_BU_OPERATION, -1);
            return;
        }

        if (CheckCanOperator(seat, static_cast<CardLogic::OperationType>(operation)) == true)
        {
            if (gang_seatno_ == seat->seat_no())
            {
                seat->data()->hand_cards_->AddCard(card);
                RemoveBuCard(seat, card);
            }
            else
            {
                DCHECK(gang_seatno_ != Table::INVALID_SEAT);
                auto gang_seat = owner_->table_obj()->GetBySeatNo(gang_seatno_);
                DCHECK(gang_seat != nullptr);
                ChangeBuCard(gang_seat, card);
            }
            bu_card_operation_ = static_cast<CardLogic::OperationType>(operation);
            bu_card_operator_seatno = seat->seat_no();
            now_operation_func_ = GetOperatorFunc(static_cast<CardLogic::OperationType>(operation), seat, card_name);
        }
    }
    else
    {
        if (gang_seatno_ == seat->seat_no())
        {
            DCHECK(seat->data()->card_now_operates_.size() == 2);
            auto card1 = seat->data()->card_now_operates_[0].first;
            auto card2 = seat->data()->card_now_operates_[1].first;
            auto& seats = owner_->table_obj()->GetSeats();
            for (auto iter : seats)
            {
                if (iter->seat_no() == seatno)
                {
                    continue;
                }

                auto operation1 = GetOtherPlayerOperations(iter->seat_no(), card1);
                auto operation2 = GetOtherPlayerOperations(iter->seat_no(), card2);
                if (operation1.size() == 0 && operation2.size() == 0)
                {
                    continue;
                }
                iter->data()->card_now_operates_.push_back(std::make_pair(card1, operation1));
                iter->data()->card_now_operates_.push_back(std::make_pair(card2, operation2));

                NotifyBuOperation(iter->seat_no());
            }
        }
    }

    assistx2::Stream stream(SERVER_RESPONSE_BU_OPERATION);
    stream.Write(seatno);
    stream.Write(operation);
    stream.Write(card_name);
    stream.Write(gang_seatno_);
    stream.End();
    if (seatno == gang_seatno_)
    {
        owner_->BroadCast(stream);
    }
    else
    {
        player->SendTo(stream);
    }

    seat->data()->card_now_operates_.clear();
    seat->data()->cs_now_operate_.clear();

    if (static_cast<CardLogic::OperationType>(operation) == CardLogic::HUPAI_OPERA)
    {
        DCHECK(now_operation_func_ != nullptr);
        now_operation_func_(seat, card_name);
        hu_players_.push_back(seat->seat_no());
    }

    if (IsNoGangOperator() == true && hu_players_.size() != 0)
    {
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        return;
    }

    if (IsNoGangOperator() != true) return;

    bu_card_operation_ = CardLogic::NULL_OPERA;
    bu_card_operator_seatno = Table::INVALID_SEAT;
    if (now_operation_func_ != nullptr)
    {
        now_operation_func_(seat, card_name);
    }
    else
    {
        DCHECK(active_player_ != Table::INVALID_SEAT);
        auto nextseat = owner_->table_obj()->next_seat(active_player_);
        DCHECK(nextseat != nullptr);

        BroadCastNextPlayer(nextseat->seat_no());

        owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, nextseat);
    }
}

void ChangShaRoomImpl::OnMintangOperation(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto operation_id = packet->Read<std::string>();
    auto operation_type = packet->Read<std::int32_t>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnMintangOperation: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    ClearMingtangOperation(seat, static_cast<MingTangOperationType>(operation_type));
   
    if (static_cast<MingTangOperationType>(operation_type) != MingTangOperationType::CANCLE_OPERA)
    {
        owner_->BroadCast(*packet);
    }

    auto is_have_operator = false;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->data()->now_mingtang_operate_.size() != 0)
        {
            is_have_operator = true;
            if (iter->seat_no() == seatno)
            {
                NotifyMTOperation(iter->seat_no(), iter->data()->now_mingtang_operate_);
            }
        }
    }
    
    if (is_have_operator == false)
    {
        auto operations = GetOperations(active_player_);
        if (operations.size() == 0)
        {
            operations.emplace(CardLogic::PLAY_OPERA, OperationsInfo());
        }

        NotifyOperation(active_player_, operations);
    }
}

void ChangShaRoomImpl::OnHaidiManYou(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto operation = packet->Read<std::int32_t>();
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnMintangOperation: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    if (operation == 0)
    {
        auto next_seat = owner_->table_obj()->next_seat(seatno);
        DCHECK(next_seat != nullptr);
        DCHECK(manyou_start_seatno_ != Table::INVALID_SEAT);
        if (manyou_start_seatno_ == next_seat->seat_no())
        {
            owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        }
        else
        {
            NotifyHaidiManYou(next_seat->seat_no());
        }
    }
    else
    {
        haidicard_owner_seatno_ = seatno;
        NotifyHaidiCard();
    }
}

void ChangShaRoomImpl::OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnHu: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    if (!CheckOperation(seat, CardLogic::HUPAI_OPERA, now_operation_func_))
    {
        LOG(ERROR) << "OnHu:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::HUPAI_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    if (now_operation_func_ == nullptr)
    {
        LOG(ERROR) << "OnGang:now_operation_func_ is nullptr roomid:=" << owner_->scene_id()
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::GANG_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    seat->data()->cs_now_operate_.clear();

    now_operation_func_(seat, std::string(""));

    hu_players_.push_back(seatno);

    ClearOperation(CardLogic::PENG_OPERA);
    ClearOperation(CardLogic::CHI_OPERA);

    if (IsNoOperator() == true)
    {
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
    }
    else
    {
        auto& seats = owner_->table_obj()->GetSeats();
        for (auto iter : seats)
        {
            auto res = iter->data()->cs_now_operate_.size();
            if (res != 0)
            {
                NotifyOperation(iter->seat_no(),iter->data()->cs_now_operate_);
            }
        }
    }
}

void ChangShaRoomImpl::OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto operation_id = packet->Read<std::string>();

    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    if (operation_id != seat->data()->operation_id_)
    {
        LOG(ERROR) << "OnCancle: roomid:=" << owner_->scene_id() << ",mid:=" << player->uid()
            << ",operation_id: -->" << operation_id << " != " << seat->data()->operation_id_;
        //SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, -1);
        return;
    }

    std::function<void(Seat*, std::string)> func;
    if (!CheckOperation(seat, CardLogic::CANCLE_OPERA, func))
    {
        LOG(ERROR) << "OnCancle:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::CANCLE_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, -1);
        return;
    }

    if (CheckOperation(seat, CardLogic::HUPAI_OPERA,func) == true)
    {
        SetGuoCard(seat);
    }

    SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, 0);

    if (seat->data()->cs_now_operate_.size() > 0)
    {
        seat->data()->cs_now_operate_.clear();
    }
    else
    {
        return;
    }

    if (IsNoOperator() == true && hu_players_.size() != 0)
    {
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        return;
    }

    if (IsNoOperator() != true) return;

    if ( now_operation_func_ == nullptr )
    {
        if (active_player_ == seatno)
        {
            if (seat->data()->is_ting == true)
            {
                DCHECK(seat->data()->mo_card_ != nullptr);
                DoPlay(seat, seat->data()->mo_card_->getName());
            }
            else
            {
                std::map<CardLogic::OperationType, OperationsInfo> opera;
                opera.emplace(CardLogic::PLAY_OPERA, OperationsInfo());
                NotifyOperation(seatno, opera);
            }
        }
        else
        {
            DCHECK(active_player_ != Table::INVALID_SEAT);
            auto nextseat = owner_->table_obj()->next_seat(active_player_);
            DCHECK(nextseat != nullptr);

            BroadCastNextPlayer(nextseat->seat_no());

            owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, nextseat);
        }
    }
    else
    {
        now_operation_func_(seat,std::string("test"));
    }
}

bool ChangShaRoomImpl::CheckOperation(Seat* seat, CardLogic::OperationType operate,
        std::function<void(Seat*, std::string)>& func)
{
    func = nullptr;
    for (auto iter : seat->data()->cs_now_operate_)
    {
        if (iter.first == CardLogic::OperationType::PLAY_OPERA &&
            operate != iter.first)
        {
            return false;
        }
    }

    if (operate == CardLogic::CANCLE_OPERA)
    {
        return true;
    }

    auto iter = seat->data()->cs_now_operate_.find(operate);
    if (iter != seat->data()->cs_now_operate_.end())
    {
        func = iter->second.cb_func_;
        return true;
    }

    return false;
}

std::map<CardLogic::OperationType, OperationsInfo>
    ChangShaRoomImpl::GetOperations(std::int32_t seatno, bool checkhu)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    std::map<CardLogic::OperationType, OperationsInfo> operations;
    
    auto res = CheckHu(nullptr, seat);
    if (res == true && checkhu == true)
    {
        OperationsInfo info;
        info.cb_func_ = std::bind(&ChangShaRoomImpl::DoZiMoHu,this, seat, std::string(""));
        operations.emplace(CardLogic::HUPAI_OPERA, info);
    }
    res = CheckBuZhang(nullptr, seat);
    if (res == true)
    {
        if (CheckCanGang(nullptr, seat) == true)
        {
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoGang, this, seat, std::string(""));
            operations.emplace(CardLogic::GANG_OPERA, info);
        }
        OperationsInfo info;
        info.cb_func_ = std::bind(&ChangShaRoomImpl::DoBuZhang, this, seat, std::string(""));
        operations.emplace(CardLogic::BU_OPERA, info);
    }

    return operations;
}

std::map<CardLogic::OperationType, OperationsInfo> 
ChangShaRoomImpl::GetOperations(std::int32_t seatno, std::shared_ptr<Card> card)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    std::map<CardLogic::OperationType, OperationsInfo> operations;

    auto res = CheckHu(card, seat);
    if (res == true)
    {
        OperationsInfo info;
        info.cb_func_ = std::bind(&ChangShaRoomImpl::DoZiMoHu, this, seat, card->getName());
        operations.emplace(CardLogic::HUPAI_OPERA, info);
    }
    res = CheckBuZhang(card, seat);
    if (res == true)
    {
        if (CheckCanGang(card, seat) == true)
        {
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoGang, this, seat, card->getName());
            operations.emplace(CardLogic::GANG_OPERA, info);
        }
        OperationsInfo info;
        info.cb_func_ = std::bind(&ChangShaRoomImpl::DoBuZhang, this, seat, card->getName());
        operations.emplace(CardLogic::BU_OPERA, info);
    }

    return operations;
}

void ChangShaRoomImpl::BroadCastPlayedCard(std::int32_t seatno, std::shared_ptr<Card> card)
{
    assistx2::Stream stream(SERVER_BROADCAST_PLAYED_CARD);
    stream.Write(seatno);
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);
}

void ChangShaRoomImpl::FindOperatorPlayer(std::int32_t seatno, std::shared_ptr<Card> card)
{
    DLOG(INFO) << "FindOperatorPlayer: roomid:=" << owner_->scene_id()
        << ",seatno:=" << seatno << ",card_name:=" << card->getName();

    
    auto next_seat = owner_->table_obj()->next_seat(seatno);
    DCHECK(next_seat != nullptr);

    auto has_operator = false;

    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        std::map<CardLogic::OperationType, OperationsInfo> operatios;
        if (iter->seat_no() == seatno)
        {
            continue;
        }
        if (iter->seat_no() == next_seat->seat_no() && iter->data()->is_ting == false)
        {
            auto res = card_logic_->CheckChi(card, iter->data()->hand_cards_);
            if (res == true)
            {
                OperationsInfo info;
                info.cb_func_ = std::bind(&ChangShaRoomImpl::DoChi, this, iter, card->getName());
                info.card = card->getName();
                operatios.emplace(CardLogic::CHI_OPERA, info);
            }
        }
        auto res = card_logic_->CheckPeng(card, iter->data()->hand_cards_);
        if (res == true && iter->data()->is_ting == false)
        {
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoPeng, this, iter, card->getName());
            info.card = card->getName();
            operatios.emplace(CardLogic::PENG_OPERA,info);
        }
        res = CheckBuZhang(card, iter);
        if (res == true)
        {
            if (CheckCanGang(card, iter) == true)
            {
                OperationsInfo info;
                info.cb_func_ = std::bind(&ChangShaRoomImpl::DoGang, this, iter, card->getName());
                info.card = card->getName();
                operatios.emplace(CardLogic::GANG_OPERA, info);
            }
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoBuZhang, this, iter, card->getName());
            info.card = card->getName();
            operatios.emplace(CardLogic::BU_OPERA, info);
        }
        res = CheckHu(card, iter) ;
        if (res == true)
        {
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoDianPaoHu, this, iter, card->getName());
            info.card = card->getName();
            operatios.emplace(CardLogic::HUPAI_OPERA,info);
        }
        if (operatios.size() != 0u)
        {
            has_operator = true;
            NotifyOperation(iter->seat_no(), operatios);
        }
    }

    if (has_operator == false)
    {
        auto nextseat = owner_->table_obj()->next_seat(seatno);
        BroadCastNextPlayer(nextseat->seat_no());

        owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, nextseat);
    }
}

bool ChangShaRoomImpl::FindGongGang(Seat* seat, std::shared_ptr<Card> card)
{
    if (card != nullptr)
    {
        return false;
    }

    for (auto iter : seat->data()->operated_cards_)
    {
        if (iter.second != PENG)
        {
            continue;
        }
        for (auto card_iter : seat->data()->hand_cards_->hand_cards())
        {
            if (iter.first->getFace() == card_iter->getFace() &&
                iter.first->getType() == card_iter->getType())
            {
                return true;
            }
        }
    }
    return false;
}

bool ChangShaRoomImpl::FindQiangGangHu(std::int32_t seatno, std::shared_ptr<Card> card)
{
    auto hu_count = 0;
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
            hu_count += 1;
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoDianPaoHu, this, iter, card->getName());
            info.card = card->getName();
            std::map<CardLogic::OperationType, OperationsInfo> opera;
            opera.emplace(CardLogic::HUPAI_OPERA, info);
            NotifyOperation(iter->seat_no(), opera);
        }
    }

    if (hu_count > 0)
    {
        return true;
    }

    return false;
}

void ChangShaRoomImpl::MoCard(std::shared_ptr<Agent > player)
{
    auto cardcount = static_cast<std::int32_t>(
        owner_->card_generator()->count());
    if (cardcount == 0)
    {
        hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        return ;
    }
    if (cardcount == 1)
    {
        manyou_start_seatno_ = player->seat_no();
        NotifyHaidiManYou(manyou_start_seatno_);
        return;
    }

    NotifyMoCard(player);
}

void ChangShaRoomImpl::BuCard(std::shared_ptr<Agent > player)
{
    auto cardcount = static_cast<std::int32_t>(
        owner_->card_generator()->count());
    if (cardcount == 0)
    {
        hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        return;
    }

    NotifyMoCard(player);
}

void ChangShaRoomImpl::NotifyMoCard(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    last_mo_seat_ = seatno;

    auto card = owner_->card_generator()->Pop();
    assistx2::Stream package(SERVER_NOTIFY_MO_CARD);
    package.Write(seatno);
    package.Write(card->getName());
    package.Write(static_cast<std::int32_t>(owner_->card_generator()->count()));
    package.End();

    player->SendTo(package);

    assistx2::Stream stream(SERVER_BROADCAST_PLAYER_MO);
    stream.Write(seatno);
    stream.Write(static_cast<std::int32_t>(owner_->card_generator()->count()));
    stream.End();

    owner_->BroadCast(stream, player);

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::MO_OPERA, card);

    DLOG(INFO) << "MoCard: roomid:=" << owner_->scene_id() << ",mid:="
        << player->uid() << ",card_name:=" << card->getName();

    seat->data()->mo_card_ = card;
    seat->data()->hand_cards_->AddCard(card);

    auto operations = GetOperations(seatno);
    if (operations.size() == 0 && seat->data()->is_ting == false)
    {
        operations.emplace(CardLogic::PLAY_OPERA, OperationsInfo());
    }

    if (operations.size() != 0)
    {
        NotifyOperation(seatno, operations);
    }
    else
    {
        if (seat->data()->is_ting == true)
        {
            DoPlay(seat, card->getName());
        }
    }
}

void ChangShaRoomImpl::SendErrorCode(std::shared_ptr<Agent > player,
    const std::int16_t cmd, const std::int32_t err)
{
    assistx2::Stream stream(cmd);
    stream.Write(err);
    stream.End();

    player->SendTo(stream);
}

void ChangShaRoomImpl::CalculateScore(std::int32_t seatno, std::int32_t score)
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

void ChangShaRoomImpl::ZhaNiao()
{
    auto zhama_num_tmp = zhama_num_;
    auto card_count = owner_->card_generator()->count();
    if (static_cast<std::int32_t>(card_count)
        < zhama_num_)
    {
        zhama_num_tmp = card_count;
    }

    for (std::int32_t i = 0; i < zhama_num_tmp; ++i)
    {
        auto card = owner_->card_generator()->Pop();
        vec_niao_.push_back(card);
    }

    DLOG(INFO) << "ZhaNiao: roomid:=" << owner_->scene_id() << ",niao:" << vec_niao_;
    Cards temp_niao = vec_niao_;
    for (auto iter_card : vec_niao_)
    {
        auto& seats = owner_->table_obj()->GetSeats();
        for (auto iter : seats)
        {
            auto num = iter->seat_no() - zhama_start_seatno_;//玩家和胡家的距离
            if (iter_card->getFace() == Card::Face::HongZ &&
                iter->seat_no() == zhama_start_seatno_)
            {
                AddZhongMaSeats(iter->seat_no(), iter_card);
                Common::RemoveCard(iter_card, temp_niao);
            }
            else if (iter_card->getFace() == Card::Face::HongZ)
            {
                continue;
            }
            else if (iter_card->getFace() == (Card::Face::One + num) ||
                iter_card->getFace() == (Card::Face::Five + num) ||
                iter_card->getFace() == (Card::Face::Nine + num))
            {
                AddZhongMaSeats(iter->seat_no(), iter_card);
                Common::RemoveCard(iter_card, temp_niao);
            }
            else
            {
                continue;
            }
        }
    }

    for (auto iter : temp_niao)
    {
        AddZhongMaSeats(0, iter);
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

std::int32_t ChangShaRoomImpl::GetZhongNiaoNum(std::int32_t seatno)
{
    auto it = zhong_ma_seats_.find(seatno);
    if (it != zhong_ma_seats_.end())
    {
        return it->second.size();
    }

    return 0;
}

void ChangShaRoomImpl::ClearGameData()
{
    active_player_ = Table::INVALID_SEAT;
    now_played_seatno_ = Table::INVALID_SEAT;
    last_mo_seat_ = Table::INVALID_SEAT;
    beiqiang_seat_ = Table::INVALID_SEAT;
    hu_players_.clear();
    zhong_ma_seats_.clear();
    hu_card_ = nullptr;
    vec_niao_.clear();
    now_operation_func_ = nullptr;
    hu_message_.clear();
    zhama_start_seatno_ = Table::INVALID_SEAT;
    manyou_start_seatno_ = Table::INVALID_SEAT;
    gang_seatno_ = Table::INVALID_SEAT;
    bu_card_operator_seatno = Table::INVALID_SEAT;
    haidicard_owner_seatno_ = Table::INVALID_SEAT;
    now_manyou_seatno_ = Table::INVALID_SEAT;
    bu_card_operation_ = CardLogic::NULL_OPERA;
    hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->ClearNowGameData();
        auto state = ((iter->seat_player_state() & Seat::PLAYER_STATUS_NET_CLOSE) | Seat::PLAYER_STATUS_WAITING);
        iter->set_seat_player_state(state);
    }
}

void ChangShaRoomImpl::ClearRoomData()
{
    active_player_ = Table::INVALID_SEAT;
    now_played_seatno_ = Table::INVALID_SEAT;
    last_mo_seat_ = Table::INVALID_SEAT;
    beiqiang_seat_ = Table::INVALID_SEAT;
    zhama_num_ = 0;
    room_operation_ = 0;
    hu_players_.clear();
    zhong_ma_seats_.clear();
    hu_card_ = nullptr;
    vec_niao_.clear();
    now_operation_func_ = nullptr;
    hu_message_.clear();
    zhama_start_seatno_ = Table::INVALID_SEAT;
    manyou_start_seatno_ = Table::INVALID_SEAT;
    gang_seatno_ = Table::INVALID_SEAT;
    bu_card_operator_seatno = Table::INVALID_SEAT;
    haidicard_owner_seatno_ = Table::INVALID_SEAT;
    now_manyou_seatno_ = Table::INVALID_SEAT;
    bu_card_operation_ = CardLogic::NULL_OPERA;
    hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->ClearAllData();
        iter->set_seat_player_state(Seat::PLAYER_STATUS_WAITING);
    }
}

void ChangShaRoomImpl::NotifyTableData(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);
    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    auto mo_card = seat->data()->mo_card_;
    auto& hand_cards = seat->data()->hand_cards_->hand_cards();
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
        auto& seat_hand_cards = iter->data()->hand_cards_->hand_cards();
        stream.Write(static_cast<std::int32_t>(seat_hand_cards.size()));
        //存牌区的牌
        auto& operated_cards = iter->data()->operated_cards_;
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

    auto leave_time = owner_->room_conifg_data()->bettime - (time(nullptr) - now_active_time_);
    auto show_time = leave_time > 0?leave_time:0;
    stream.Write(static_cast<std::int32_t>(show_time));
    stream.End();

    player->SendTo(stream);

    for (auto iter : hu_message_)
    {
        player->SendTo(iter);
    }

    if (now_manyou_seatno_ != Table::INVALID_SEAT)
    {
        NotifyHaidiManYou(now_manyou_seatno_);
    }

    if (seat->data()->now_mingtang_operate_.size() != 0)
    {
        NotifyMTOperation(seatno, seat->data()->now_mingtang_operate_);
    }

    if (seat->data()->cs_now_operate_.size() != 0)
    {
        NotifyOperation(seatno, seat->data()->cs_now_operate_);
    }
    if (seat->data()->card_now_operates_.size() != 0)
    {
        NotifyBuOperation(seatno);
    }
}

bool ChangShaRoomImpl::CheckHu(const std::shared_ptr<Card> card, Seat* seat)
{
    if (card != nullptr)
    {
        for (auto iter : seat->data()->guo_cards_)
        {
            if (card->getFace() == iter->getFace() &&
                card->getType() == iter->getType())
            {
                return false;
            }
        }
    }
    auto res = card_logic_->CheckHu_258(card, seat->data()->hand_cards_);
    if (res == true)
    {
        return true;
    }
    if (seat->data()->operated_cards_.size() == 4u && 
        card_logic_->CheckHu(card, seat->data()->hand_cards_) == true)
    {
        return true;
    }
    if (IsQingYiSe(card, seat) == true &&
        card_logic_->CheckHu(card, seat->data()->hand_cards_) == true)
    {
        return true;
    }
    if (seat->data()->operated_cards_.size() != 0u)
    {
        return res;
    }
    res = card_logic_->CheckHu7Dui(card, seat->data()->hand_cards_);
    
    return res;
}


bool ChangShaRoomImpl::CheckBuZhang(const std::shared_ptr<Card> card, Seat* seat)
{
    auto res = card_logic_->CheckGang(card, seat->data()->hand_cards_);
    if (res == true)
    {
        return true;
    }
    if  (FindGongGang(seat, card) == true)
    {
        return true;
    }
    
    return false;
}

bool ChangShaRoomImpl::CheckCanGang(const std::shared_ptr<Card> card, Seat* seat)
{
    if (owner_->card_generator()->count() < 2)
    {
        return false;
    }

    std::vector<std::shared_ptr<Card>> gang_cards;

    if (card == nullptr)
    {
        gang_cards = GetGangCard(seat);
    }
    else
    {
        gang_cards.push_back(card);
    }

    for (auto iter : gang_cards)
    {
        if (card == nullptr)
        {
            CHECK(seat->data()->hand_cards_->RemoveCard(iter) == true);
        }
        if (card_logic_->CheckTing_258(seat->data()->hand_cards_) == true)
        {
            if (card == nullptr) seat->data()->hand_cards_->AddCard(iter);
            return true;
        }
        if (seat->data()->operated_cards_.size() == 4u &&
            card_logic_->CheckTing(seat->data()->hand_cards_) == true)
        {
            if (card == nullptr) seat->data()->hand_cards_->AddCard(iter);
            return true;
        }
        if (IsQingYiSe(nullptr,seat) == true &&
            card_logic_->CheckTing(seat->data()->hand_cards_) == true)
        {
            if (card == nullptr) seat->data()->hand_cards_->AddCard(iter);
            return true;
        }
        if (seat->data()->operated_cards_.size() != 0u)
        {
            if (card == nullptr) seat->data()->hand_cards_->AddCard(iter);
            continue;
        }
        if (card_logic_->CheckTing7Dui(seat->data()->hand_cards_) == true)
        {
            if (card == nullptr) seat->data()->hand_cards_->AddCard(iter);
            return true;
        }

        if (card == nullptr) seat->data()->hand_cards_->AddCard(iter);
    }

    return false;
}

void ChangShaRoomImpl::IncrMingtang(Seat* seat, MingTangType type)
{
    DCHECK(seat != nullptr);

    auto iter = seat->data()->mingtang_types_.find(type);
    if (iter != seat->data()->mingtang_types_.end())
    {
        iter->second += 1;
    }
    else
    {
        seat->data()->mingtang_types_.emplace(type, 1);
    }
}

void ChangShaRoomImpl::RoomAccount()
{
    auto& seats = owner_->table_obj()->GetSeats();
    if (static_cast<std::int32_t>(seats.size()) !=
        owner_->table_obj()->player_count())
    {
        return;
    }
    assistx2::Stream stream(SERVER_BROADCAST_ROOM_ACCOUNT);
    stream.Write(static_cast<std::int32_t>(owner_->room_owner()));
    stream.Write(owner_->table_obj()->player_count());
    for (auto iter : seats)
    {
        if (iter->player() != nullptr)
        {
            stream.Write(iter->seat_no());
            stream.Write(static_cast<std::int32_t>(iter->player()->uid()));
            stream.Write(iter->player()->member_fides()->name());
            stream.Write(iter->player()->member_fides()->icon());
            stream.Write(iter->data()->dahu_zimo_);
            stream.Write(iter->data()->xiaohu_zimo_);
            stream.Write(iter->data()->dahu_dianpao_);
            stream.Write(iter->data()->xiaohu_dianpao_);
            stream.Write(iter->data()->dahu_jiepao_);
            stream.Write(iter->data()->xiaohu_jiepao_);
            stream.Write(0);
            stream.Write(iter->data()->seat_score_);
        }
    }
    stream.Write(owner_->RoomDataToString(false));
    stream.End();

    owner_->BroadCast(stream, nullptr, true);
}

bool ChangShaRoomImpl::IsZhuangXianCalculateScore()
{
    if ((room_operation_ & 0x01) == 0x01)
    {
        return true;
    }
    return false;
}

void ChangShaRoomImpl::DoPlay(Seat* seat, std::string card_name)
{
    DCHECK(seat != nullptr);

    auto seatno = seat->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto card = CardFactory::MakeMajiangCard(card_name);
    DCHECK(card != nullptr);

    DLOG(INFO) << "OnPlay:PlayCard: roomid:=" << owner_->scene_id()
        << ",mid:=" << seat->player()->uid() << ",card_name:=" << card_name;

    auto res = seat->data()->hand_cards_->RemoveCard(card);
    if (res == false)
    {
        LOG(ERROR) << "OnPlay:PlayCard Failed roomid:=" << owner_->scene_id()
            << ",mid:=" << seat->player()->uid() << ",card_name:=" << card->getName() << ",not in hand_cards";
        SendErrorCode(seat->player(), CLIENT_REQUEST_ONPLAY, -1);
        return;
    }

    seat->data()->played_cards_.push(card);
    seat->data()->cs_now_operate_.clear();
    seat->data()->mo_card_ = nullptr;

    now_played_seatno_ = seatno;

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::PLAY_OPERA, card);

    auto value = now_played_seatno_;
    if (seat->data()->is_ting == true)
    {
        value += 100;
    }

    BroadCastPlayedCard(value, card);

    FindOperatorPlayer(now_played_seatno_, card);
}

void ChangShaRoomImpl::DoChi(Seat* seat, std::string card_name)
{
    auto card = CardFactory::MakeMajiangCard(card_name);
    if (card == nullptr)
    {
        LOG(ERROR) << "DoChi: card == nullptr card_name:=" << card_name
            << "roomid: = " << owner_->scene_id() << ",mid:=" << seat->player()->uid();

        SendErrorCode(seat->player(), SERVER_BROADCAST_CHI_CARD, -1);
        return;
    }

    DLOG(INFO) << "DoChi: roomid:=" << owner_->scene_id()
        << ",mid:=" << seat->player()->uid() << ",card_name:=" << card_name;

    auto card1 = CardFactory::MakeMajiangCard(static_cast<Card::Face>(card->getFace() + 1), card->getType());
    auto card2 = CardFactory::MakeMajiangCard(static_cast<Card::Face>(card->getFace() + 2), card->getType());
    if (card1 == nullptr || card2 == nullptr)
    {
        LOG(ERROR) << "DoChi: card error card_name:=" << card_name
            << "roomid: = " << owner_->scene_id() << ",mid:=" << seat->player()->uid();

        SendErrorCode(seat->player(), SERVER_BROADCAST_CHI_CARD, -1);
        return;
    }

    DCHECK(now_played_seatno_ != Table::INVALID_SEAT);
    auto played_seat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
    DCHECK(played_seat != nullptr);

    auto played_card = played_seat->data()->played_cards_.top();
    played_seat->data()->played_cards_.pop();

    if (played_card->getFace() == card->getFace() &&
        played_card->getType() == card->getType())
    {
        CHECK(seat->data()->hand_cards_->RemoveCard(card1) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card2) == true);
    }
    else if (played_card->getFace() == card1->getFace() &&
        played_card->getType() == card1->getType())
    {
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card2) == true);
    }
    else if (played_card->getFace() == card2->getFace() &&
        played_card->getType() == card2->getType())
    {
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card1) == true);
    }
    else
    {
        LOG(ERROR) << "DoChi: no such played_card:=" << played_card->getName()
            << "roomid: = " << owner_->scene_id() << ",mid:=" << seat->player()->uid();

        SendErrorCode(seat->player(), SERVER_BROADCAST_CHI_CARD, -1);
        return;
    }

    seat->data()->operated_cards_.insert(std::make_pair(card, CHI));

    assistx2::Stream stream(SERVER_BROADCAST_CHI_CARD);
    stream.Write(seat->seat_no());
    stream.Write(card->getName());
    stream.Write(played_seat->seat_no());
    stream.Write(played_card->getName());
    stream.End();

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seat->seat_no(), CardLogic::CHI_OPERA, card);

    BroadCastNextPlayer(seat->seat_no());

    std::map<CardLogic::OperationType, OperationsInfo> opera;
    opera.emplace(CardLogic::PLAY_OPERA, OperationsInfo());
    NotifyOperation(seat->seat_no(), opera);
}

void ChangShaRoomImpl::DoPeng(Seat* seat, std::string card_name)
{
    now_operation_func_ = nullptr;

    auto seatno = seat->seat_no();

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

    assistx2::Stream stream(SERVER_BROADCAST_PENG_CARD);
    stream.Write(seatno);
    stream.Write(played_seat->seat_no());
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::PENG_OPERA, card);

    BroadCastNextPlayer(seatno);

    auto operation = GetOperations(seatno,false);
    if (operation.size() == 0)
    {
        operation.emplace(CardLogic::PLAY_OPERA, OperationsInfo());
    }
    NotifyOperation(seatno, operation);
}

void ChangShaRoomImpl::DoGang(Seat* seat, std::string card_name)
{
    DLOG(INFO) << "DoGang: roomid:=" << owner_->scene_id()
        << ",mid:=" << seat->player()->uid() << ",card_name:=" << card_name;

    now_operation_func_ = nullptr;

    auto seatno = seat->seat_no();
    OperCardsType type = NULL_TYPE;
    std::shared_ptr<Card> card = nullptr;
    Seat* operaseat = nullptr;
    if (active_player_ == seat->seat_no())
    {
        operaseat = seat;
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
            card = GetGGangCard(seat,card_recv);
            DCHECK(card != nullptr);
            type = GGANG;
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
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

        type = MGANG;

        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
    }

    CHECK(card != nullptr);

    if (type == GGANG)
    {
        auto res = FindQiangGangHu(seatno, card);
        if (res == true)
        {
            now_operation_func_ = std::bind(&ChangShaRoomImpl::DoGongGang, this, seat, card->getName());
            SendErrorCode(seat->player(), SERVER_BROADCAST_GANG_CARD, -2);
            return;
        }
        auto iter = std::find_if(seat->data()->operated_cards_.begin(), seat->data()->operated_cards_.end(),
            [card](const std::pair<std::shared_ptr<Card>, OperCardsType> value) {
            return card->getName() == value.first->getName();
        });
        if (iter != seat->data()->operated_cards_.end())
        {
            seat->data()->operated_cards_.erase(iter);
        }
    }

    seat->data()->is_ting = true;
    seat->data()->operated_cards_.insert(std::make_pair(card, type));

    assistx2::Stream stream(SERVER_BROADCAST_GANG_CARD);
    stream.Write(seatno);
    stream.Write(static_cast<std::int32_t>(type));
    stream.Write(operaseat->seat_no());
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::GANG_OPERA, card);

    BroadCastNextPlayer(seatno);

    seat->data()->gang_card_ = card;
    gang_seatno_ = seatno;
    //补两张
    owner_->NewTimer(1, SceneTimerContext::MAJIANG_GANG_BUPAI , seat);
}

void ChangShaRoomImpl::DoBuZhang(Seat* seat, std::string card_name)
{
    DLOG(INFO) << "DoBuZhang: roomid:=" << owner_->scene_id()
        << ",mid:=" << seat->player()->uid() << ",card_name:=" << card_name;

    now_operation_func_ = nullptr;

    auto seatno = seat->seat_no();
    OperCardsType type = NULL_TYPE;
    std::shared_ptr<Card> card = nullptr;
    Seat* operaseat = nullptr;
    if (active_player_ == seat->seat_no())
    {
        operaseat = seat;
        auto card_recv = CardFactory::MakeMajiangCard(card_name);
        if (card_recv != nullptr &&
            seat->data()->hand_cards_->card_count(card_recv) == 4)
        {
            type = ABU;
            card = card_recv;
        }
        else
        {
            auto cards_info = seat->data()->hand_cards_->hand_cards_info();
            for (auto iter : cards_info)
            {
                if (iter.num == 4)
                {
                    type = ABU;
                    card = iter.card;
                    break;
                }
            }
        }
        if (type == NULL_TYPE)
        {
            card = GetGGangCard(seat, card_recv);
            DCHECK(card != nullptr);
            type = GBU;
            CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
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

        type = MBU;

        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
        CHECK(seat->data()->hand_cards_->RemoveCard(card) == true);
    }

    CHECK(card != nullptr);

    if (type == GBU)
    {
        auto iter = std::find_if(seat->data()->operated_cards_.begin(), seat->data()->operated_cards_.end(),
            [card](const std::pair<std::shared_ptr<Card>, OperCardsType> value) {
            return card->getName() == value.first->getName();
        });
        if (iter != seat->data()->operated_cards_.end())
        {
            seat->data()->operated_cards_.erase(iter);
        }
    }

    seat->data()->operated_cards_.insert(std::make_pair(card, type));

    assistx2::Stream stream(SERVER_BROADCAST_BU_CARD);
    stream.Write(seatno);
    stream.Write(static_cast<std::int32_t>(type));
    stream.Write(operaseat->seat_no());
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::BU_OPERA, card);

    BroadCastNextPlayer(seatno);

    owner_->NewTimer(1, SceneTimerContext::MAJIANG_BUPAI, seat);
}

void ChangShaRoomImpl::DoGongGang(Seat* seat, std::string card_name)
{
    DLOG(INFO) << "DoGongGang: roomid:=" << owner_->scene_id()
        << ",mid:=" << seat->player()->uid() << ",card_name:=" << card_name;

    now_operation_func_ = nullptr;

    auto seatno = seat->seat_no();

    auto active_player_seat = owner_->table_obj()->GetBySeatNo(active_player_);
    DCHECK(active_player_seat != nullptr);
    auto card = active_player_seat->data()->mo_card_;
    DCHECK(card != nullptr);

    auto iter = std::find_if(seat->data()->operated_cards_.begin(), seat->data()->operated_cards_.end(),
        [card](const std::pair<std::shared_ptr<Card>, OperCardsType> value) {
        return card->getName() == value.first->getName();
    });
    if (iter != seat->data()->operated_cards_.end())
    {
        seat->data()->operated_cards_.erase(iter);
    }
   
    seat->data()->is_ting = true;
    seat->data()->operated_cards_.insert(std::make_pair(card, GGANG));

    assistx2::Stream stream(SERVER_BROADCAST_GANG_CARD);
    stream.Write(seatno);
    stream.Write(static_cast<std::int32_t>(GGANG));
    stream.Write(active_player_);
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::GANG_OPERA, card);

    BroadCastNextPlayer(seatno);

    seat->data()->gang_card_ = card;
    gang_seatno_ = seatno;
    //补两张
    owner_->NewTimer(1, SceneTimerContext::MAJIANG_GANG_BUPAI, seat);
}

void ChangShaRoomImpl::DoZiMoHu(Seat* seat, std::string card_name)
{
    auto seatno = seat->seat_no();

    hu_type_ = PrivateRoom::HuType::ZIMOHU;


    hu_card_ = seat->data()->mo_card_;
    if (hu_card_ != nullptr)
    {
        CHECK(seat->data()->hand_cards_->RemoveCard(hu_card_) == true);
    }

    auto mingtang = SetMingTang(hu_card_, seat);
    if (mingtang.size() == 0)
    {
        CalculateScore(seat->seat_no(), 2);
        seat->data()->xiaohu_zimo_ += 1;
    }
    else
    {
        CalculateScore(seat->seat_no(), 6* mingtang.size());
        seat->data()->dahu_zimo_ += 1;
    }
    NotifyMingTangType(seatno,mingtang);

    assistx2::Stream stream(SERVER_BROADCAST_HU_CARD);
    stream.Write(seatno);
    auto& hand_cards = seat->data()->hand_cards_->hand_cards();
    stream.Write(static_cast<std::int32_t>(hand_cards.size()));
    for (auto it : hand_cards)
    {
        stream.Write(it->getName());
    }
    if (hu_card_ == nullptr)
    {
        stream.Write(std::string(""));
    }
    else
    {
        stream.Write(hu_card_->getName());
    }

    stream.End();

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seatno, 
        CardLogic::HUPAI_OPERA, hu_card_,MINGTANG_ZIMOHU);

    zhama_start_seatno_ = seatno;
}

void ChangShaRoomImpl::DoDianPaoHu(Seat* seat, std::string card_name)
{
    hu_card_ = CardFactory::MakeMajiangCard(card_name);
    DCHECK(hu_card_ != nullptr);

    Seat* dianpaoseat = nullptr;
    if (seat->data()->is_qianggang_hu_ == false)
    {
        dianpaoseat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
        DCHECK(dianpaoseat != nullptr);
        hu_type_ = PrivateRoom::HuType::DIANPAOHU;
        beiqiang_seat_ = now_played_seatno_;
    }
    else
    {
        dianpaoseat = owner_->table_obj()->GetBySeatNo(active_player_);
        DCHECK(dianpaoseat != nullptr);
        hu_type_ = PrivateRoom::HuType::QINAGGANGHU;
        beiqiang_seat_ = active_player_;
        seat->data()->game_score_ += 6;
        dianpaoseat->data()->game_score_ -= 6;
    }

    auto mingtang = SetMingTang(hu_card_, seat);
    if (seat->data()->is_qianggang_hu_ == true)
    {
        IncrMingtang(seat, MINGTANG_QIANGGANG_HU);
    }
    if (mingtang.size() == 0 && seat->data()->is_qianggang_hu_ == false)
    {
        seat->data()->game_score_ += 2;
        dianpaoseat->data()->game_score_ -= 2;
        seat->data()->xiaohu_jiepao_ += 1;
        dianpaoseat->data()->xiaohu_dianpao_ += 1;
    }
    else
    {
        seat->data()->game_score_ += 6* mingtang.size();
        dianpaoseat->data()->game_score_ -= 6 * mingtang.size();
        seat->data()->dahu_jiepao_ += 1;
        dianpaoseat->data()->dahu_dianpao_ += 1;
    }

    NotifyMingTangType(seat->seat_no(),mingtang);

    IncrMingtang(seat, MINGTANG_JIEPAO);
    IncrMingtang(dianpaoseat, MINGTANG_DIANPAO);

    assistx2::Stream stream(SERVER_BROADCAST_QIANGGANG_HU);
    stream.Write(seat->seat_no());
    stream.Write(dianpaoseat->seat_no());
    stream.Write(hu_card_->getName());
    auto& hand_cards = seat->data()->hand_cards_->hand_cards();
    stream.Write(static_cast<std::int32_t>(hand_cards.size()));
    for (auto it : hand_cards)
    {
        stream.Write(it->getName());
    }
    stream.End();

    hu_message_.push_back(stream);

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seat->seat_no(), 
        CardLogic::HUPAI_OPERA, hu_card_, MINGTANG_DIANPAO);
}

void ChangShaRoomImpl::DoHaidiHu(Seat* seat, std::string card_name)
{
    hu_type_ = PrivateRoom::HuType::HAIDIHU;
    hu_card_ = CardFactory::MakeMajiangCard(card_name);
    DCHECK(hu_card_ != nullptr);

    seat->data()->dahu_zimo_ += 1;

    auto mingtang = SetMingTang(hu_card_, seat);

    CalculateScore(seat->seat_no(), 6 * (mingtang.size() + 1));

    NotifyMingTangType(seat->seat_no(),mingtang);

    assistx2::Stream stream(SERVER_BROADCAST_HAIDI_HU);
    stream.Write(seat->seat_no());
    stream.Write(hu_card_->getName());
    auto& hand_cards = seat->data()->hand_cards_->hand_cards();
    stream.Write(static_cast<std::int32_t>(hand_cards.size()));
    for (auto it : hand_cards)
    {
        stream.Write(it->getName());
    }
    stream.End();

    owner_->BroadCast(stream);

    IncrMingtang(seat, MINGTANG_HAI_DI_PAO);

    GameDataManager::getInstance()->OnOperation(owner_, seat->seat_no(), 
        CardLogic::HUPAI_OPERA, hu_card_, MINGTANG_HAI_DI_LAO_YUE);

    zhama_start_seatno_ = seat->seat_no();
}

void ChangShaRoomImpl::DoHaidiPao(Seat* seat, std::string card_name)
{
    hu_type_ = PrivateRoom::HuType::HAIDIPAO;
    hu_card_ = CardFactory::MakeMajiangCard(card_name);
    DCHECK(hu_card_ != nullptr);

    DCHECK(haidicard_owner_seatno_ != Table::INVALID_SEAT);
    auto dianpaoseat = owner_->table_obj()->GetBySeatNo(haidicard_owner_seatno_);
    beiqiang_seat_ = haidicard_owner_seatno_;

    seat->data()->dahu_jiepao_ += 1;
    dianpaoseat->data()->dahu_dianpao_ += 1;

    auto mingtang = SetMingTang(hu_card_, seat);

    seat->data()->game_score_ += 6 * (mingtang.size() + 1);
    dianpaoseat->data()->game_score_ -= 6 * (mingtang.size() + 1);

    NotifyMingTangType(seat->seat_no(), mingtang);

    IncrMingtang(seat, MINGTANG_JIEPAO);
    IncrMingtang(dianpaoseat, MINGTANG_DIANPAO);

    assistx2::Stream stream(SERVER_BROADCAST_HAIDI_PAO);
    stream.Write(seat->seat_no());
    stream.Write(haidicard_owner_seatno_);
    stream.Write(hu_card_->getName());
    auto& hand_cards = seat->data()->hand_cards_->hand_cards();
    stream.Write(static_cast<std::int32_t>(hand_cards.size()));
    for (auto it : hand_cards)
    {
        stream.Write(it->getName());
    }
    stream.End();

    hu_message_.push_back(stream);

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seat->seat_no(), 
        CardLogic::HUPAI_OPERA, hu_card_, MINGTANG_HAI_DI_PAO);

    IncrMingtang(seat, MINGTANG_HAI_DI_LAO_YUE);
}

void ChangShaRoomImpl::DoGangHu(Seat* seat, std::string card_name)
{
    hu_type_ = PrivateRoom::HuType::GANGHU;
    hu_card_ = CardFactory::MakeMajiangCard(card_name);
    DCHECK(hu_card_ != nullptr);

    seat->data()->dahu_zimo_ += 1;

    auto mingtang = SetMingTang(hu_card_, seat);

    CalculateScore(seat->seat_no(), 6 * (mingtang.size() + 1));

    NotifyMingTangType(seat->seat_no(), mingtang);

    assistx2::Stream stream(SERVER_BROADCAST_GANG_HU);
    stream.Write(seat->seat_no());
    stream.Write(hu_card_->getName());
    auto& hand_cards = seat->data()->hand_cards_->hand_cards();
    stream.Write(static_cast<std::int32_t>(hand_cards.size()));
    for (auto it : hand_cards)
    {
        stream.Write(it->getName());
    }
    stream.End();

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seat->seat_no(), 
        CardLogic::HUPAI_OPERA, hu_card_, MINGTANG_GANGSHANG_KAIHUA);

    IncrMingtang(seat, MINGTANG_GANGSHANG_KAIHUA);

    zhama_start_seatno_ = seat->seat_no();
}

void ChangShaRoomImpl::DoGangPao(Seat* seat, std::string card_name)
{
    hu_type_ = PrivateRoom::HuType::GANGPAO;
    hu_card_ = CardFactory::MakeMajiangCard(card_name);
    DCHECK(hu_card_ != nullptr);

    DCHECK(gang_seatno_ != Table::INVALID_SEAT);
    auto dianpaoseat = owner_->table_obj()->GetBySeatNo(gang_seatno_);
    beiqiang_seat_ = gang_seatno_;

    seat->data()->dahu_jiepao_ += 1;
    dianpaoseat->data()->dahu_dianpao_ += 1;

    auto mingtang = SetMingTang(hu_card_, seat);

    seat->data()->game_score_ += 6 * (mingtang.size() + 1);
    dianpaoseat->data()->game_score_ -= 6 * (mingtang.size() + 1);

    NotifyMingTangType(seat->seat_no(), mingtang);

    IncrMingtang(seat, MINGTANG_JIEPAO);
    IncrMingtang(dianpaoseat, MINGTANG_DIANPAO);

    assistx2::Stream stream(SERVER_BROADCAST_GANG_PAO);
    stream.Write(seat->seat_no());
    stream.Write(gang_seatno_);
    stream.Write(hu_card_->getName());
    auto& hand_cards = seat->data()->hand_cards_->hand_cards();
    stream.Write(static_cast<std::int32_t>(hand_cards.size()));
    for (auto it : hand_cards)
    {
        stream.Write(it->getName());
    }
    stream.End();

    hu_message_.push_back(stream);

    owner_->BroadCast(stream);

    GameDataManager::getInstance()->OnOperation(owner_, seat->seat_no(), 
        CardLogic::HUPAI_OPERA, hu_card_, MINGTANG_GANGSHANG_PAO);

    IncrMingtang(seat, MINGTANG_GANGSHANG_PAO);
}

void ChangShaRoomImpl::AddZhongMaSeats(const std::int32_t seatno, const std::shared_ptr<Card> card)
{
    auto it = zhong_ma_seats_.find(seatno);
    if (it != zhong_ma_seats_.end())
    {
        it->second.push_back(card);
    }
    else
    {
        zhong_ma_seats_.insert(std::make_pair(seatno,
            std::vector<std::shared_ptr<Card>>(1, card)));
    }
}

bool ChangShaRoomImpl::IsNoOperator()
{
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->data()->cs_now_operate_.size() != 0)
        {
            return false;
        }
    }

    return true;
}

bool ChangShaRoomImpl::IsNoGangOperator()
{
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->data()->card_now_operates_.size() != 0)
        {
            return false;
        }
    }

    return true;
}

void ChangShaRoomImpl::ClearOperation(CardLogic::OperationType operate)
{
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        auto it = iter->data()->cs_now_operate_.find(operate);
        if (it != iter->data()->cs_now_operate_.end())
        {
            iter->data()->cs_now_operate_.erase(it);
        }
    }
}

bool ChangShaRoomImpl::IsQingYiSe(std::shared_ptr<Card> card, Seat* seat)
{
    std::set<Card::Type> setCardType;
    auto& hand_cards = seat->data()->hand_cards_->hand_cards_info();
    for (auto iter : hand_cards)
    {
        setCardType.insert(iter.card->getType());
    }
    auto& operatored_cards = seat->data()->operated_cards_;
    for (auto iter : operatored_cards)
    {
        setCardType.insert(iter.first->getType());
    }

    if (card != nullptr)
    {
        setCardType.insert(card->getType());
    }

    if (setCardType.size() == 1u)
    {
        return true;
    }

    return false;
}

bool ChangShaRoomImpl::IsPengPeng(std::shared_ptr<Card> card, Seat* seat)
{
    bool isAllPeng = true;
    auto& operatored_cards = seat->data()->operated_cards_;
    for (auto iter : operatored_cards)
    {
        if (iter.second < PENG || iter.second > GGANG)
        {
            isAllPeng = false;
            break;
        }
    }

    return isAllPeng;
}

bool ChangShaRoomImpl::IsJiangJiang(std::shared_ptr<Card> card, Seat* seat)
{
    bool isAll258 = true;
    auto& hand_cards = seat->data()->hand_cards_->hand_cards_info();
    for (auto iter : hand_cards)
    {
        if (iter.card->getFace() != Card::Face::Two ||
            iter.card->getFace() != Card::Face::Five ||
            iter.card->getFace() != Card::Face::Eight)
        {
            isAll258 = false;
            break;
        }
    }
    auto& operatored_cards = seat->data()->operated_cards_;
    for (auto iter : operatored_cards)
    {
        if (iter.first->getFace() != Card::Face::Two ||
            iter.first->getFace() != Card::Face::Five ||
            iter.first->getFace() != Card::Face::Eight)
        {
            isAll258 = false;
            break;
        }
    }

    if (card != nullptr && isAll258 == true)
    {
        if (card->getFace() != Card::Face::Two ||
            card->getFace() != Card::Face::Five ||
            card->getFace() != Card::Face::Eight)
        {
            isAll258 = false;
        }
    }

    return isAll258;
}

std::vector< std::shared_ptr<Card>> ChangShaRoomImpl::GetGangCard(Seat* seat)
{
    std::vector< std::shared_ptr<Card>> gang_cards;
    auto& hand_cards = seat->data()->hand_cards_->hand_cards_info();
    for (auto iter : hand_cards)
    {
        if (iter.num == 4)
        {
            gang_cards.push_back(iter.card);
        }
        for (auto it : seat->data()->operated_cards_)
        {
            if (it.second != PENG)
            {
                continue;
            }
            if (it.first->getFace() == iter.card->getFace() &&
                it.first->getType() == iter.card->getType())
            {
                gang_cards.push_back(it.first);
            }
        }
    }
    
    return gang_cards;
}

std::map<CardLogic::OperationType, OperationsInfo>
    ChangShaRoomImpl::GetOtherPlayerOperations(std::int32_t seatno, std::shared_ptr<Card> card)
{
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    auto next_seat = owner_->table_obj()->next_seat(seatno);
    DCHECK(next_seat != nullptr);

    std::map<CardLogic::OperationType, OperationsInfo> operatios;
    if (seat->seat_no() == next_seat->seat_no() && seat->data()->is_ting == false)
    {
        auto res = card_logic_->CheckChi(card, seat->data()->hand_cards_);
        if (res == true)
        {
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoChi, this, seat, card->getName());
            info.card = card->getName();
            operatios.emplace(CardLogic::CHI_OPERA, info);
        }
    }
    auto res = card_logic_->CheckPeng(card, seat->data()->hand_cards_);
    if (res == true && seat->data()->is_ting == false)
    {
        OperationsInfo info;
        info.cb_func_ = std::bind(&ChangShaRoomImpl::DoPeng, this, seat, card->getName());
        info.card = card->getName();
        operatios.emplace(CardLogic::PENG_OPERA, info);
    }
    res = CheckBuZhang(card, seat);
    if (res == true)
    {
        if (CheckCanGang(card, seat) == true)
        {
            OperationsInfo info;
            info.cb_func_ = std::bind(&ChangShaRoomImpl::DoGang, this, seat, card->getName());
            info.card = card->getName();
            operatios.emplace(CardLogic::GANG_OPERA, info);
        }
        OperationsInfo info;
        info.cb_func_ = std::bind(&ChangShaRoomImpl::DoBuZhang, this, seat, card->getName());
        info.card = card->getName();
        operatios.emplace(CardLogic::BU_OPERA, info);
    }
    res = CheckHu(card, seat);
    if (res == true)
    {
        OperationsInfo info;
        info.cb_func_ = std::bind(&ChangShaRoomImpl::DoDianPaoHu, this, seat, card->getName());
        info.card = card->getName();
        operatios.emplace(CardLogic::HUPAI_OPERA, info);
    }
      
    return operatios;
}

void ChangShaRoomImpl::NotifyBuZhang(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    DCHECK(owner_->card_generator()->count() >= 2);

    auto card1 = owner_->card_generator()->Pop();
    auto card2 = owner_->card_generator()->Pop();

    NotifyPlayBuCard(seat,card1,card2);

    last_mo_seat_ = seatno;

    auto operation1 = GetOperations(seatno,card1);
    auto operation2 = GetOperations(seatno,card2);

    if (operation1.size() == 0 && operation2.size() == 0)
    {
        auto is_have_operation = false;
        auto& seats = owner_->table_obj()->GetSeats();
        for (auto iter : seats)
        {
            if (iter->seat_no() == seatno)
            {
                continue;
            }
            operation1 = GetOtherPlayerOperations(iter->seat_no(), card1);
            operation2 = GetOtherPlayerOperations(iter->seat_no(), card2);
            if (operation1.size() == 0 && operation2.size() == 0)
            {
                continue;
            }
            is_have_operation = true;
            iter->data()->card_now_operates_.push_back(std::make_pair(card1, operation1));
            iter->data()->card_now_operates_.push_back(std::make_pair(card2, operation2));

            NotifyBuOperation(iter->seat_no());
        }
        if (is_have_operation == true) return;
    }
    else
    {
        seat->data()->card_now_operates_.push_back(std::make_pair(card1, operation1));
        seat->data()->card_now_operates_.push_back(std::make_pair(card2, operation2));

        return NotifyBuOperation(seatno);
    }

    DCHECK(active_player_ != Table::INVALID_SEAT);
    auto nextseat = owner_->table_obj()->next_seat(active_player_);
    DCHECK(nextseat != nullptr);

    BroadCastNextPlayer(nextseat->seat_no());

    owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, nextseat);
}

void ChangShaRoomImpl::NotifyBuOperation(std::int32_t seatno)
{
    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    DCHECK(gang_seatno_ != Table::INVALID_SEAT);
    auto gang_seat = owner_->table_obj()->GetBySeatNo(gang_seatno_);
    DCHECK(gang_seat != nullptr);

    DCHECK(gang_seat->data()->gang_card_ != nullptr);

    std::stringstream ss;
    ss << seat->player()->uid() << "_" << owner_->scene_id() << "_" << (rand() % 100000 + 1);
    seat->data()->operation_id_ = ss.str();

    auto& operations = seat->data()->card_now_operates_;
    assistx2::Stream stream(SERVER_NOTIFY_BU_OPERATION);
    stream.Write(seat->data()->operation_id_);
    stream.Write(gang_seatno_);
    stream.Write(gang_seat->data()->gang_card_->getName());
    stream.Write(static_cast<std::int32_t>(operations.size()));
    for (auto iter : operations)
    {
        stream.Write(iter.first->getName());
        stream.Write(static_cast<std::int32_t>(iter.second.size()));
        for (auto it : iter.second)
        {
            stream.Write(static_cast<std::int32_t>(it.first));
            DLOG(INFO) << "NotifyBuOperation: roomid:=" << owner_->scene_id() <<
                ",mid:=" << seat->player()->uid() << ",operate:=" << it.first << ",card:=" << iter.first->getName();
        }
    }
    stream.End();

    seat->player()->SendTo(stream);
}

std::function<void(Seat*, std::string)> ChangShaRoomImpl::GetOperatorFunc(
    CardLogic::OperationType operate,Seat* seat, std::string card)
{
    switch (operate)
    {
    case CardLogic::PENG_OPERA:
        return std::bind(&ChangShaRoomImpl::DoPeng, this, seat, card);
        break;
    case CardLogic::GANG_OPERA:
        return std::bind(&ChangShaRoomImpl::DoGang, this, seat, card);
        break;
    case CardLogic::HUPAI_OPERA:
    {
        if (seat->seat_no() == gang_seatno_)
        {   
            return std::bind(&ChangShaRoomImpl::DoGangHu, this, seat, card);
        }
        else
        {
            return std::bind(&ChangShaRoomImpl::DoGangPao, this, seat, card);
        }
    }
        break;
    case CardLogic::CHI_OPERA:
        return std::bind(&ChangShaRoomImpl::DoChi, this, seat, card);
        break;
    case CardLogic::BU_OPERA:
        return std::bind(&ChangShaRoomImpl::DoBuZhang, this, seat, card);
        break;
    default:
        break;
    }

    return nullptr;
}

void ChangShaRoomImpl::NotifyPlayBuCard(Seat* seat,std::shared_ptr<Card> card1, std::shared_ptr<Card> card2)
{
    std::vector<std::shared_ptr<Card>> cards;
    if (card1 != nullptr)
    {
        cards.push_back(card1);
        seat->data()->played_cards_.push(card1);
    }
   
    if (card2 != nullptr)
    {
        cards.push_back(card2);
        seat->data()->played_cards_.push(card2);
    }
    now_played_seatno_ = seat->seat_no();

    assistx2::Stream stream(SERVER_NOTIFY_PLAY_BUCARD);
    stream.Write(seat->seat_no());
    stream.Write(static_cast<std::int32_t>(cards.size()));
    for (auto iter : cards)
    {
        stream.Write(iter->getName());
    }
    stream.Write(owner_->card_generator()->count());
    stream.End();

    owner_->BroadCast(stream);
}

void ChangShaRoomImpl::RemoveBuCard(Seat* seat, std::shared_ptr<Card> card)
{
    auto bu_card = seat->data()->played_cards_.top();
    if (bu_card->getFace() == card->getFace() &&
        bu_card->getType() == card->getType())
    {
        DLOG(INFO) << "RemoveBuCard: card:=" << bu_card->getName();
        seat->data()->played_cards_.pop();
    }
    else
    {
        seat->data()->played_cards_.pop();
        DLOG(INFO) << "RemoveBuCard: card:=" << seat->data()->played_cards_.top()->getName();
        seat->data()->played_cards_.pop();
        seat->data()->played_cards_.push(bu_card);
    }
}

void ChangShaRoomImpl::ChangeBuCard(Seat* seat, std::shared_ptr<Card> card)
{
    auto bu_card = seat->data()->played_cards_.top();
    if (bu_card->getFace() == card->getFace() &&
        bu_card->getType() == card->getType())
    {
        //do nothing
    }
    else
    {
        seat->data()->played_cards_.pop();
        auto tempcard = seat->data()->played_cards_.top();
        seat->data()->played_cards_.pop();

        seat->data()->played_cards_.push(bu_card);
        seat->data()->played_cards_.push(tempcard);
    }
}

std::int32_t ChangShaRoomImpl::GetOperationLevel(CardLogic::OperationType operate)
{
    switch (operate)
    {
    case CardLogic::PENG_OPERA:
        return 1;
    case CardLogic::GANG_OPERA:
        return 1;
    case CardLogic::HUPAI_OPERA:
        return 2;
    case CardLogic::CHI_OPERA:
        return 0;
    case CardLogic::BU_OPERA:
        return 1;
    default:
        break;
    }

    return 0;
}

bool ChangShaRoomImpl::CheckCanOperator(Seat* seat,CardLogic::OperationType operate)
{
    DLOG(INFO) << "CheckCanOperator bu_card_operation_:=" << bu_card_operation_
        << ",bu_card_operator_seatno:=" << bu_card_operator_seatno << ",gang_seatno_:=" 
        << gang_seatno_ << ",now_seat:=" << seat->seat_no() << ",now_operate:=" << operate;

    if (bu_card_operation_ == CardLogic::NULL_OPERA ||
        bu_card_operator_seatno == Table::INVALID_SEAT ||
        operate == CardLogic::HUPAI_OPERA)
    {
        return true;
    }

    if (GetOperationLevel(bu_card_operation_) == GetOperationLevel(operate))
    {
        if ((seat->seat_no() - gang_seatno_) < (bu_card_operator_seatno - gang_seatno_))
        {
            return true;
        }
    }
    else
    {
        if (GetOperationLevel(operate) > GetOperationLevel(bu_card_operation_))
        {
            return true;
        }
    }

    return false;
}

std::set<MingTangOperationType> ChangShaRoomImpl::GetQSMTOperation(Seat* seat)
{
    DCHECK(seat != nullptr);
    std::set<Card::Type> setCardType;
    std::set<MingTangOperationType> operations;

    bool isHave258 = false;
    auto iKeCardCount = 0;//刻牌的个数
    auto& hand_cards = seat->data()->hand_cards_->hand_cards_info();
    for (auto& iter : hand_cards)
    {
        if (iter.num == 4)
        {
            operations.insert(SIXI_OPERA);
        }
        if (iter.card->getFace() == Card::Face::Two ||
            iter.card->getFace() == Card::Face::Five ||
            iter.card->getFace() == Card::Face::Eight)
        {
            isHave258 = true;
        }
        if (iter.num >= 3)
        {
            iKeCardCount += 1;
        }
        setCardType.insert(iter.card->getType());
    }

    if (isHave258 == false)
    {
        operations.insert(BANBAN_OPERA);
    }
    if (iKeCardCount >= 2)
    {
        operations.insert(SIXSIXSHUN_OPERA);
    }
    if (setCardType.size() == 2)
    {
        operations.insert(QUEYISE_OPERA);
    }
    
    return operations;
}

void ChangShaRoomImpl::ClearMingtangOperation(Seat* seat, MingTangOperationType operation)
{
    CalculateMingTang(seat, operation);
    if (operation == MingTangOperationType::CANCLE_OPERA)
    {
        seat->data()->now_mingtang_operate_.clear();
    }
    else
    {
        auto& operations = seat->data()->now_mingtang_operate_;
        auto iter = std::find_if(operations.begin(), operations.end(), [operation](MingTangOperationType value) {
            return operation == value;
        });
        if (iter != operations.end())
        {
            operations.erase(iter);
        }
    }
}

void ChangShaRoomImpl::NotifyHaidiManYou(std::int32_t seatno)
{
    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    now_manyou_seatno_ = seatno;

    std::stringstream ss;
    ss << seat->player()->uid() << "_" << owner_->scene_id() << "_" << (rand() % 100000 + 1);
    seat->data()->operation_id_ = ss.str();

    assistx2::Stream stream(SERVER_NOTIFY_HAIDIMANYOU);
    stream.Write(seat->data()->operation_id_);
    stream.End();

    seat->player()->SendTo(stream);
}

void ChangShaRoomImpl::NotifyHaidiCard()
{
    DCHECK(owner_->card_generator()->count() == 1);
    auto card = owner_->card_generator()->Pop();

    assistx2::Stream stream(SERVER_BROADCAST_HAIDI_CARD);
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);

    last_mo_seat_ = haidicard_owner_seatno_;

    DCHECK(haidicard_owner_seatno_ != Table::INVALID_SEAT);
    auto seat = owner_->table_obj()->GetBySeatNo(haidicard_owner_seatno_);
    DCHECK(seat != nullptr);

    auto res = CheckHu(card, seat);
    if (res == true)
    {
        //海底胡
        DoHaidiHu(seat, card->getName());
        hu_players_.push_back(seat->seat_no());
    }
    else
    {
        auto& seats = owner_->table_obj()->GetSeats();
        for (auto iter : seats)
        {
            if (iter->seat_no() == haidicard_owner_seatno_)
            {
                continue;
            }
            auto res = CheckHu(card, iter);
            if (res == true)
            {
                //海底炮
                DoHaidiPao(iter, card->getName());
                hu_players_.push_back(iter->seat_no());
            }
        }
    }

    owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
}


std::vector<MingTangType> ChangShaRoomImpl::SetMingTang(std::shared_ptr<Card> card, Seat* seat)
{
    std::vector<MingTangType> mintang;
    if (last_mo_seat_ == Table::INVALID_SEAT)
    {
        if (now_played_seatno_ == Table::INVALID_SEAT)
        {
            mintang.push_back(MINGTANG_TIAN_HU);
            IncrMingtang(seat, MINGTANG_TIAN_HU);
        }
        else
        {
            mintang.push_back(MINGTANG_DI_HU);
            IncrMingtang(seat, MINGTANG_DI_HU);
        }
    }

    if (IsJiangJiang(card, seat) == true)
    {
        mintang.push_back(MINGTANG_JIANG_JIANG_HU);
        IncrMingtang(seat, MINGTANG_JIANG_JIANG_HU);
    }

    if (seat->data()->operated_cards_.size() == 4u)
    {
        if (IsPengPeng(card, seat) == true)
        {
            mintang.push_back(MINGTANG_PENG_PENG_HU);
            IncrMingtang(seat, MINGTANG_PENG_PENG_HU);
        }
        mintang.push_back(MINGTANG_QUAN_QIU_REN);
        IncrMingtang(seat, MINGTANG_QUAN_QIU_REN);
    }
    if (IsQingYiSe(card, seat) == true)
    {
        mintang.push_back(MINGTANG_QING_YI_SE);
        IncrMingtang(seat, MINGTANG_QING_YI_SE);
    }
    if (seat->data()->operated_cards_.size() != 0u)
    {
        return mintang;
    }
    auto res = card_logic_->CheckHu7Dui(card, seat->data()->hand_cards_);
    if (res == true)
    {
        auto count = 0;
        auto& hand_cards = seat->data()->hand_cards_->hand_cards_info();
        for (auto iter : hand_cards)
        {
            if (iter.num == 4)
            {
                count += 1;
            }
        }
        if (count >= 2)
        {
            mintang.push_back(MINGTANG_SHUANG_HAOHUA_QIXIAODUI);
            IncrMingtang(seat, MINGTANG_SHUANG_HAOHUA_QIXIAODUI);
        }
        else if (count == 1)
        {
            mintang.push_back(MINGTANG_HAOHUA_QIXIAODUI);
            IncrMingtang(seat, MINGTANG_HAOHUA_QIXIAODUI);
        }
        else
        {
            mintang.push_back(MINGTANG_QI_XIAODUI);
            IncrMingtang(seat, MINGTANG_QI_XIAODUI);
        }
    }

    return mintang;
}

void ChangShaRoomImpl::NotifyMingTangType(std::int32_t seatno,std::vector<MingTangType>& mingtang)
{
    if (mingtang.size() == 0)
    {
        mingtang.push_back(MINGTANG_PING_HU);
    }
    assistx2::Stream stream(SERVER_BROADCAST_MINGTANG);
    stream.Write(seatno);
    stream.Write(static_cast<std::int32_t>(mingtang.size()));
    for (auto iter : mingtang)
    {
        stream.Write(static_cast<std::int32_t>(iter));
    }
    stream.End();

    owner_->BroadCast(stream);
}

void ChangShaRoomImpl::CalculateMingTang(Seat* seat, MingTangOperationType operation)
{
    switch (operation)
    {
    case SIXI_OPERA:
        IncrMingtang(seat, MINGTANG_DA_SI_XI);
        break;
    case SIXSIXSHUN_OPERA:
        IncrMingtang(seat, MINGTANG_SIX_SIX_SHUN);
        break;
    case BANBAN_OPERA:
        IncrMingtang(seat, MINGTANG_BAN_BAN_HU);
        break;
    case QUEYISE_OPERA:
        IncrMingtang(seat, MINGTANG_QUE_YI_SE);
        break;
    case CANCLE_OPERA:
        return;
    default:
        break;
    }
    CalculateScore(seat->seat_no(), 2);
}

void ChangShaRoomImpl::SetGuoCard(Seat* seat)
{
    if (seat->seat_no() == active_player_)
    {
        return;
    }
    auto dianpaoseat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
    DCHECK(dianpaoseat != nullptr);
    auto card = dianpaoseat->data()->played_cards_.top();
    seat->data()->guo_cards_.push_back(card);
}

std::shared_ptr<Card> ChangShaRoomImpl::GetGGangCard(Seat* seat, std::shared_ptr<Card> card)
{
    for (auto iter : seat->data()->operated_cards_)
    {
        if (iter.second != PENG)
        {
            continue;
        }
        if (card != nullptr)
        {
            if (card->getFace() == iter.first->getFace() &&
                card->getType() == iter.first->getType())
            {
                return card;
            }
        }
        else
        {
            for (auto card_iter : seat->data()->hand_cards_->hand_cards())
            {
                if (iter.first->getFace() == card_iter->getFace() &&
                    iter.first->getType() == card_iter->getType())
                {
                    return card_iter;
                }
            }
        }
    }

    return nullptr;
}
