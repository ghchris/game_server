#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include <json_spirit_writer_template.h>
#include <assistx2/json_wrapper.h>
#include "hzmajiangroom.h"
#include "table.h"
#include "seat.h"
#include "cardgenerator.h"
#include "seatdata.h"
#include "hzmajianglogic.h"
#include "agent.h"
#include "cardgroup.h"
#include "gameconfigdata.h"
#include "scenetimer.h"
#include "memberfides.pb.h"
#include "gamedatamanager.h"
#include "logserver.h"
#include "common.h"

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

const static std::int32_t YI_MA_QUAN_ZHONG = 1;//一码全中

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
    void ZhaNiao_YiMaQuanZhong();
    bool CheckHu(const std::shared_ptr<Card> card,Seat* seat);
    void IncrMingtang(Seat* seat,MingTangType type);
    std::int32_t GetZhongNiaoNum(std::int32_t seatno);
    std::int32_t GetZhongNiaoScore(std::int32_t seatno);
    void ClearGameData();
    void ClearRoomData();
    void NotifyTableData(std::shared_ptr<Agent > player);
    bool NotHaveHongZhong(Seat* seat);
    void RoomAccount();
    void CheckQSHu(std::int32_t seatno, std::vector<CardLogic::OperationType>& operation);
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
    std::int32_t room_operation_ = 0;//房间选项 1为7对胡
    std::vector<std::int32_t> zhama_value_{0,2,3,4,6 };
    std::vector<std::shared_ptr<Card>> vec_niao_;
    PrivateRoom::HuType hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    time_t now_active_time_ = 0;
    bool is_yi_ma_quan_zhong_ = 0;
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
    if (value == YI_MA_QUAN_ZHONG)
    {
        pImpl_->zhama_num_ = 1;
        pImpl_->is_yi_ma_quan_zhong_ = true;
    }
    else
    {
        pImpl_->is_yi_ma_quan_zhong_ = false;
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
}

void HzMajiangRoom::set_operation(const std::int32_t value)
{
    pImpl_->room_operation_ = value;
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

void HzMajiangRoom::OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context)
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
    case SceneTimerContext::MAJIANG_GAMEOVER:
        OnGameOver();
        break;
    default:
        break;
    }
}

void HzMajiangRoom::OnGameStart()
{
    PrivateRoom::OnGameStart();

    card_generator()->Reset(CardGenerator::Type::HZ_MAJIANG);

    auto& seats = table_obj()->GetSeats();
    for (auto iter : seats)
    {
        Cards cards;
        assistx2::Stream package(SERVER_BROADCAST_ON_DEAL);
        if (banker_seatno() == iter->seat_no())
        {
            package.Write(14);
            auto card = card_generator()->Pop(cards);
            package.Write(card->getName());
            cards.push_back(card);
        }
        else
        {
            package.Write(13);
        }
        for (std::int32_t i = 0; i < 13; ++i)
        {
            auto card = card_generator()->Pop(cards);
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

    auto operations = pImpl_->GetOperations(banker_seatno());
    //是否起手胡
    pImpl_->CheckQSHu(banker_seatno(), operations);

    if (operations.size() == 0)
    {
        operations.push_back(CardLogic::PLAY_OPERA);
    }
 
    pImpl_->NotifyOperation(banker_seatno(), operations);
}

void HzMajiangRoom::OnGameOver()
{
    DLOG(INFO) << "OnGameOver: roomid:=" << scene_id()
        << ",type:=" << static_cast<std::int32_t>(pImpl_->hu_type_);

    auto now_banker_seatno = banker_seatno();

    if (pImpl_->hu_type_ == PrivateRoom::HuType::ZIMOHU)
    {
        DCHECK(pImpl_->hu_players_.size() > 0u);
        auto hu_seatno = pImpl_->hu_players_[0];
        DCHECK(hu_seatno != Table::INVALID_SEAT);
        auto sum_score = 0;
        if (pImpl_->is_yi_ma_quan_zhong_ == false)
        {
            pImpl_->ZhaNiao();
            auto num = pImpl_->GetZhongNiaoNum(hu_seatno);
            sum_score = 2 + num * 2;
        }
        else
        {
            pImpl_->ZhaNiao_YiMaQuanZhong();
            auto num = pImpl_->GetZhongNiaoScore(hu_seatno);
            sum_score = 2 + num;
        }
        
        pImpl_->CalculateScore(hu_seatno, sum_score);
        set_banker_seatno( hu_seatno );
    }
    else if (pImpl_->hu_type_ == PrivateRoom::HuType::QINAGGANGHU)
    {
        if (pImpl_->is_yi_ma_quan_zhong_ == false)
        {
            pImpl_->ZhaNiao();
        }
        else
        {
            pImpl_->ZhaNiao_YiMaQuanZhong();
        }

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
            auto sum_score = 0;
            if (pImpl_->is_yi_ma_quan_zhong_ == false)
            {
                auto num = pImpl_->GetZhongNiaoNum(iter);
                sum_score = 2 * score + 2 * num * score;
            }
            else
            {
                auto num = pImpl_->GetZhongNiaoScore(iter);
                sum_score = 2 * score + num * score;
            }

            DCHECK(pImpl_->hu_card_ != nullptr);
            if (pImpl_->NotHaveHongZhong(seat) == true &&
                 pImpl_->hu_card_->getName() != "HZ")
            {
                sum_score += 2 * score;
            }
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

std::int32_t HzMajiangRoom::Disband()
{
    pImpl_->ClearRoomData();

    return PrivateRoom::Disband();
}

void HzMajiangRoom::OnDisbandRoom(DisbandType type)
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

void HzMajiangRoom::OnReConect(std::shared_ptr<Agent > player)
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

std::string HzMajiangRoom::RoomDataToString(bool isContainTableData)
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
            auto value = Common::StringToJson(iter->data()->seat_string_data());
            seat_json.push_back(json_spirit::Pair("data", value));
            array.push_back(seat_json);
        }

        root.push_back(json_spirit::Pair("seats", array));
    }

    return json_spirit::write_string(json_spirit::Value(root));
}

void HzMajiangRoom::StringToRoomData(const std::string& str)
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
            set_zhama_num(assistx2::ToInt32(iter.value_));
            continue;
        }
    }

    PrivateRoom::StringToRoomData(str);
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
    now_active_time_ = time(nullptr);

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
    std::stringstream ss;
    ss << seat->player()->uid() << "_" << owner_->scene_id() << "_" << (rand() % 100000 + 1);
    seat->data()->operation_id_ =  ss.str() ;

    assistx2::Stream package(SERVER_NOTIFY_OPERATE);
    package.Write(seatno);
    package.Write(static_cast<std::int32_t>(operate.size()));
    for (auto iter : operate)
    {
        package.Write(static_cast<std::int32_t>(iter));
        DLOG(INFO) << "NotifyOperation: roomid:=" << owner_->scene_id() << 
            ",mid:=" << seat->player()->uid() << ",operate:=" << iter;
    }
    package.Write(seat->data()->operation_id_);
    package.End();

    seat->player()->SendTo(package);   
}

void HzMajiangRoomImpl::OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    if (!CheckOperation(seat, CardLogic::PLAY_OPERA))
    {
        LOG(ERROR) << "OnPlay:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::PLAY_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }
   
    auto card = CardFactory::MakeMajiangCard(card_name);
    if (card == nullptr)
    {
        LOG(ERROR) << "OnPlay:PlayCard Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",card_name:=" << card_name;
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }

    DLOG(INFO) << "OnPlay:PlayCard: roomid:=" << owner_->scene_id() 
        << ",mid:=" << player->uid() << ",card_name:=" << card_name;

    auto res = seat->data()->hand_cards_->RemoveCard(card);
    if (res == false)
    {
        LOG(ERROR) << "OnPlay:PlayCard Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid()<< ",card_name:=" << card_name << "not in hand_cards";
        SendErrorCode(player, CLIENT_REQUEST_ONPLAY, -1);
        return;
    }
    seat->data()->played_cards_.push(card);
    seat->data()->now_operate_.clear();
    seat->data()->mo_card_ = nullptr;

    now_played_seatno_ = seatno;

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::PLAY_OPERA, card);

    BroadCastPlayedCard(now_played_seatno_, card);

    FindOperatorPlayer(now_played_seatno_, card);
}

void HzMajiangRoomImpl::OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    if (!CheckOperation(seat, CardLogic::PENG_OPERA))
    {
        LOG(ERROR) << "OnPeng:CheckOperation Failed roomid:=" << owner_->scene_id() 
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

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::PENG_OPERA, card);

    BroadCastNextPlayer(seatno);
    
    NotifyOperation(seatno, std::vector<CardLogic::OperationType>(1,CardLogic::PLAY_OPERA));
}

void HzMajiangRoomImpl::OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    if (!CheckOperation(seat, CardLogic::GANG_OPERA))
    {
        LOG(ERROR) << "OnGang:CheckOperation Failed roomid:=" << owner_->scene_id() 
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
        auto card_recv = CardFactory::MakeMajiangCard(card_name);
        if (card_recv != nullptr && 
            seat->data()->hand_cards_->card_count(card_recv) == 4)
        {
            type = AGANG;
            card = card_recv;
            IncrMingtang(seat, MINGTANG_ANGANG);
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
                    IncrMingtang(seat, MINGTANG_ANGANG);
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
                if (iter.first->getFace() == seat->data()->mo_card_->getFace() &&
                    iter.first->getType() == seat->data()->mo_card_->getType())
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

        IncrMingtang(played_seat,MINGTANG_DIANGANG);
        IncrMingtang(seat, MINGTANG_JIEGANG);
        
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

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::GANG_OPERA, card);

    if (type == GGANG)
    {
        auto res = FindQiangGangHu(seatno,card);
        if (res == true)
        {
            IncrMingtang(seat, MINGTANG_DIANPAO);
            seat->data()->beiqiang_hu_num_ += 1;
            return;
        }
        auto iter = std::find_if(seat->data()->operated_cards_.begin(), seat->data()->operated_cards_.end(), 
            [card](const std::pair<std::shared_ptr<Card>,OperCardsType> value) {
            return card->getName() == value.first->getName();
        });
        if (iter != seat->data()->operated_cards_.end())
        {
            seat->data()->operated_cards_.erase(iter);
        }
        seat->data()->gang_gong_num_ += 1;
        IncrMingtang(seat, MINGTANG_GONGGANG);
        CalculateScore(seatno,1);
    }
    else if (type == AGANG)
    {
        seat->data()->gang_an_num_ += 1;
        CalculateScore(seatno, 2);
    }

    seat->data()->operated_cards_.insert(std::make_pair(card, type));
 
    BroadCastNextPlayer(seatno);

    owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, seat);
    //MoCard(player);
}

void HzMajiangRoomImpl::OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    if (!CheckOperation(seat, CardLogic::HUPAI_OPERA))
    {
        LOG(ERROR) << "OnHu:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::HUPAI_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONHU, -1);
        return;
    }

    seat->data()->now_operate_.clear();
    
    seat->data()->zimo_num_ += 1;
    IncrMingtang(seat, MINGTANG_ZIMOHU);

    if (NotHaveHongZhong(seat) == true)
    {
        CalculateScore(seat->seat_no(), 2);
        IncrMingtang(seat, MINGTANG_WUHONGZHONG);
    }

    hu_card_ = seat->data()->mo_card_;
    if (hu_card_ != nullptr)
    {
        CHECK(seat->data()->hand_cards_->RemoveCard(hu_card_) == true);
    }

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

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::HUPAI_OPERA, hu_card_);

    hu_players_.push_back(seatno);

    hu_type_ = PrivateRoom::HuType::ZIMOHU;
    owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
}
void HzMajiangRoomImpl::OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    if (!CheckOperation(seat, CardLogic::CANCLE_OPERA))
    {
        LOG(ERROR) << "OnCancle:CheckOperation Failed roomid:=" << owner_->scene_id() 
            << ",mid:=" << player->uid() << ",operate:=" << CardLogic::CANCLE_OPERA;
        SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, -1);
        return;
    }

    SendErrorCode(player, CLIENT_REQUEST_ONCANCLE, 0);

    if (seat->data()->now_operate_.size() > 0)
    {
        seat->data()->now_operate_.clear();
    }
    else
    {
        return;
    }

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

        owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, nextseat);
        //MoCard(nextseat->player());
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

        owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, nextseat);
        //MoCard(nextseat->player());
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
            assistx2::Stream stream(SERVER_BROADCAST_QIANGGANG_HU);
            stream.Write(iter->seat_no());
            stream.Write(seatno);
            stream.Write(card->getName());
            auto& hand_cards = iter->data()->hand_cards_->hand_cards();
            stream.Write(static_cast<std::int32_t>(hand_cards.size()));
            for (auto it : hand_cards)
            {
                stream.Write(it->getName());
            }
            stream.End();

            owner_->BroadCast(stream);

            GameDataManager::getInstance()->OnOperation(owner_, iter->seat_no(), CardLogic::HUPAI_OPERA, hu_card_);
            iter->data()->gang_hu_num_ += 1;
            IncrMingtang(iter, MINGTANG_JIEPAO);
        }
    }

    if (hu_players_.size() > 0u)
    {
        beiqiang_seat_ = seatno;
        hu_type_ = PrivateRoom::HuType::QINAGGANGHU;
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        return true;
    }

    return false;
}

void HzMajiangRoomImpl::MoCard(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    auto cardcount = static_cast<std::int32_t>(
        owner_->card_generator()->count());
    if (cardcount == zhama_num_)
    {
        hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        return ;
    }

    last_mo_seat_ = seatno;

    auto card = owner_->card_generator()->Pop(seat->data()->hand_cards_->hand_cards());
    assistx2::Stream package(SERVER_NOTIFY_MO_CARD);
    package.Write(seatno);
    package.Write(card->getName());
    package.Write(cardcount - 1);
    package.End();

    player->SendTo(package);

    assistx2::Stream stream(SERVER_BROADCAST_PLAYER_MO);
    stream.Write(seatno);
    stream.Write(cardcount - 1);
    stream.End();

    owner_->BroadCast( stream, player);

    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::MO_OPERA, card);

    DLOG(INFO) << "MoCard: roomid:=" << owner_->scene_id() << ",mid:=" 
        << player->uid() << ",card_name:=" << card->getName();

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
    for (std::int32_t i = 0; i < zhama_num_; ++i)
    {
        auto card = owner_->card_generator()->Pop();
        vec_niao_.push_back(card);
        niao_str += card->getName() + ",";
    }

    DLOG(INFO) << "ZhaNiao: roomid:=" << owner_->scene_id() << ",niao:" << niao_str;
    
    for (auto iter_card : vec_niao_)
    {
        auto& seats = owner_->table_obj()->GetSeats();
        for (auto iter : seats)
        {
            auto num = iter->seat_no() - owner_->banker_seatno();//玩家和庄家的距离
            if (iter_card->getFace() == Card::Face::HongZ &&
                iter->seat_no() == owner_->banker_seatno())
            {
                auto it = zhong_ma_seats_.find(iter->seat_no());
                if (it != zhong_ma_seats_.end())
                {
                    it->second.push_back(iter_card);
                }
                else
                {
                    zhong_ma_seats_.insert(std::make_pair(iter->seat_no(), 
                        std::vector<std::shared_ptr<Card>>(1, iter_card)));
                }
            }
            else if (iter_card->getFace() == Card::Face::HongZ)
            {
                continue;
            }
            else if (iter_card->getFace() == (Card::Face::One + num) ||
                iter_card->getFace() == (Card::Face::Five + num) ||
                iter_card->getFace() == (Card::Face::Nine + num))
            {
                auto it = zhong_ma_seats_.find(iter->seat_no());
                if (it != zhong_ma_seats_.end())
                {
                    it->second.push_back(iter_card);
                }
                else
                {
                    zhong_ma_seats_.insert(std::make_pair(iter->seat_no(),
                        std::vector<std::shared_ptr<Card>>(1, iter_card)));
                }
            }
            else
            {
                continue;
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

void HzMajiangRoomImpl::ZhaNiao_YiMaQuanZhong()
{
    auto card = owner_->card_generator()->Pop();
    vec_niao_.push_back(card);

    DLOG(INFO) << "ZhaNiao_YiMaQuanZhong: roomid:=" << owner_->scene_id() 
        << ",niao:" << card->getName();

    for (auto iter : hu_players_)
    {
        auto it = zhong_ma_seats_.find(iter);
        if (it != zhong_ma_seats_.end())
        {
            it->second.push_back(card);
        }
        else
        {
            zhong_ma_seats_.insert(std::make_pair(iter,
                std::vector<std::shared_ptr<Card>>(1, card)));
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

    DLOG(INFO) << "ZhaNiao_YiMaQuanZhong: roomid:=" << owner_->scene_id() << ss.str();
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

std::int32_t HzMajiangRoomImpl::GetZhongNiaoScore(std::int32_t seatno)
{
    auto it = zhong_ma_seats_.find(seatno);
    if (it != zhong_ma_seats_.end())
    {
        DCHECK(it->second.size() == 1);
        auto card = it->second[0];
        if (card->getFace() == Card::Face::HongZ)
        {
            return 12;
        }
        else
        {
            return (static_cast<std::int32_t>(card->getFace()) + 2);
        }
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
    vec_niao_.clear();
    hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->ClearNowGameData();
        auto state = ((iter->seat_player_state() & Seat::PLAYER_STATUS_NET_CLOSE) | Seat::PLAYER_STATUS_WAITING);
        iter->set_seat_player_state(state);
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
    room_operation_ = 0;
    hu_players_.clear();
    zhong_ma_seats_.clear();
    hu_card_ = nullptr;
    vec_niao_.clear();
    is_yi_ma_quan_zhong_ = false;
    hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
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

    if (seat->data()->now_operate_.size() != 0)
    {
        NotifyOperation(seatno, seat->data()->now_operate_);
    }
}

bool HzMajiangRoomImpl::CheckHu(const std::shared_ptr<Card> card, Seat* seat)
{
    auto res = card_logic_->CheckHu(card, seat->data()->hand_cards_);
    if (((room_operation_ & 0x01) == 0x01) && res == false)
    {
        if (seat->data()->operated_cards_.size() != 0u)
        {
            return res;
        }
        res = card_logic_->CheckHu7Dui(card, seat->data()->hand_cards_);
    }
    
    return res;
}

void HzMajiangRoomImpl::IncrMingtang(Seat* seat, MingTangType type)
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

bool HzMajiangRoomImpl::NotHaveHongZhong(Seat* seat)
{
    DCHECK(seat != nullptr);

    auto hzcard = CardFactory::MakeMajiangCard("HZ");
    DCHECK(hzcard != nullptr);

    auto hz_count = seat->data()->hand_cards_->card_count(hzcard);
    if (hz_count != 0)
    {
        return false;
    }

    auto  operated_cards = seat->data()->operated_cards_;
    for (auto iter : operated_cards)
    {
        if (iter.first->getFace() == Card::Face::HongZ &&
            iter.first->getType() == Card::Type::Zi)
        {
            return false;
        }
    }

    return true;
}

void HzMajiangRoomImpl::RoomAccount()
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
            stream.Write(iter->data()->zimo_num_);
            stream.Write(iter->data()->gang_hu_num_);
            stream.Write(iter->data()->beiqiang_hu_num_);
            stream.Write(iter->data()->gang_an_num_);
            stream.Write(iter->data()->gang_gong_num_);
            stream.Write(iter->data()->gang_ming_num_);
            stream.Write(iter->data()->gang_fang_num_);
            stream.Write(iter->data()->seat_score_);
        }
    }
    stream.Write(owner_->RoomDataToString(false));
    stream.End();

    owner_->BroadCast(stream, nullptr, true);
}

void HzMajiangRoomImpl::CheckQSHu(std::int32_t seatno, std::vector<CardLogic::OperationType>& operation)
{
    auto iter = std::find_if(operation.begin(), operation.end(),
        [](const CardLogic::OperationType value) {
        return value == CardLogic::HUPAI_OPERA;
    });
    if (iter != operation.end())
    {
        return;
    }

    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    auto card = CardFactory::MakeMajiangCard("HZ");
    auto count = seat->data()->hand_cards_->card_count(card);

    if (count == 4)
    {
        operation.push_back(CardLogic::HUPAI_OPERA);
    }
}