#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include <json_spirit_writer_template.h>
#include <assistx2/json_wrapper.h>
#include <functional>
#include "zhuanzhuanroom.h"
#include "table.h"
#include "seat.h"
#include "cardgenerator.h"
#include "seatdata.h"
#include "zzmajianglogic.h"
#include "agent.h"
#include "cardgroup.h"
#include "gameconfigdata.h"
#include "scenetimer.h"
#include "memberfides.pb.h"
#include "gamedatamanager.h"
#include "logserver.h"
#include "common.h"
#include "zzmajianglogic_twoplayer.h"

const static std::int16_t SERVER_BROADCAST_ON_DEAL = 1020;//����
const static std::int16_t SERVER_BROADCAST_NEXT_PLAYER = 1021;//�¸�������
const static std::int16_t SERVER_NOTIFY_OPERATE = 1022;//֪ͨ�û�����Ӧ�Ĳ���
const static std::int16_t CLIENT_REQUEST_ONPLAY = 1023;//�û�����
const static std::int16_t SERVER_BROADCAST_PLAYED_CARD = 1023;//�㲥�������
const static std::int16_t CLIENT_REQUEST_ONPENG = 1024;//�û���
const static std::int16_t SERVER_BROADCAST_PENG_CARD = 1024;//�㲥��
const static std::int16_t CLIENT_REQUEST_ONGANG= 1025;//�û���
const static std::int16_t SERVER_BROADCAST_GANG_CARD = 1025;//�㲥��
const static std::int16_t CLIENT_REQUEST_ONHU = 1026;//�û���
const static std::int16_t SERVER_BROADCAST_HU_CARD = 1026;//�㲥��
const static std::int16_t CLIENT_REQUEST_ONCANCLE = 1027;//�û�ȡ��
const static std::int16_t SERVER_NOTIFY_MO_CARD = 1028;//֪ͨ�û�����
const static std::int16_t SERVER_BROADCAST_QIANGGANG_HU = 1029;//���ܺ�
const static std::int16_t SERVER_BROADCAST_ZHA_NIAO = 1030;//����
const static std::int16_t SERVER_BROADCAST_GAME_ACCOUNT = 1031;//���ֽ���
const static std::int16_t SERVER_NOTIFY_PLAYER_TABLE_DATA = 1032; //���������������
const static std::int16_t SERVER_BROADCAST_PLAYER_MO = 1033;//�������㲥�ĸ����������
const static std::int16_t SERVER_BROADCAST_PLAYER_HANDCARD = 1034;//�������㲥������ϵ�����
const static std::int16_t SERVER_BROADCAST_ROOM_ACCOUNT = 1035;//�������㲥�ܽ���

const static std::int32_t TYPE_DIANPAO_HU = 0;
const static std::int32_t TYPE_ZIMO_HU = 1;

class ZhuanZhuanRoomImpl
{
public:
    ZhuanZhuanRoomImpl(ZhuanZhuanRoom* owner);
    ~ZhuanZhuanRoomImpl();
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
    void IncrMingtang(Seat* seat,MingTangType type);
    std::int32_t GetZhongNiaoNum(std::int32_t seatno);
    void ClearGameData();
    void ClearRoomData();
    void NotifyTableData(std::shared_ptr<Agent > player);
    void RoomAccount();
    bool IsZhuangXianCalculateScore();
    void CheckQSHu(std::int32_t seatno, 
        std::vector<CardLogic::OperationType>& operation);
    void AddHuPlayerOperation(const std::int32_t seatno);
    void RemoveHuPlayerOperation(const std::int32_t seatno);
    void DoPeng(Seat* seat, std::string card);
    void DoGang(Seat* seat, std::string card);
    void DoZiMoHu(Seat* seat, std::string card);
    void DoDianPaoHu(Seat* seat, std::string card);
    void DoGongGang(Seat* seat, std::string card);
    void AddZhongMaSeats(const std::int32_t seatno,const std::shared_ptr<Card> card);
    bool CanGang(const std::shared_ptr<Card> card);
    bool IsNoOperator();
    void SetGuoCard(Seat* seat);
public:
    void OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet);
public:
    ZhuanZhuanRoom* owner_;
    std::int32_t active_player_ = Table::INVALID_SEAT;//��ǰ����������λ��(���������Ҷ��ܿ���)
    std::int32_t now_operator_ = Table::INVALID_SEAT;//��ǰ������λ��(ֻ���Լ����ܿ���)
    std::int32_t now_played_seatno_ = Table::INVALID_SEAT;//��ǰ����Ƶ���λ��
    std::int32_t last_mo_seat_ = Table::INVALID_SEAT;//������Ƶ���λ��
    std::int32_t beiqiang_seat_ = Table::INVALID_SEAT;//�����ܵ���λ��
    std::shared_ptr<CardLogic> card_logic_;
    std::vector<std::int32_t> hu_players_;
    std::int32_t zhama_num_ = 0;//����ĸ��� Ĭ��ֵ0
    std::map<std::int32_t, std::vector<std::shared_ptr<Card>>> zhong_ma_seats_;
    std::shared_ptr<Card> hu_card_ = nullptr;//������
    std::int32_t room_operation_ = 0;//����ѡ�� 1Ϊׯ�� 2�߶Ժ� 4�������
    std::vector<std::int32_t> zhama_value_{0,2,3,4,6,8};
    std::vector<std::shared_ptr<Card>> vec_niao_;
    PrivateRoom::HuType hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    time_t now_active_time_ = 0;
    std::int32_t play_type_ = TYPE_DIANPAO_HU;//0 ���ں� 1 ��ģ��
    std::vector<std::int32_t> hu_player_operation_;//���ں����ȼ��ж�
    std::function<void(Seat*, std::string)> next_func_ = nullptr;
    std::int32_t zhama_start_seatno_ = Table::INVALID_SEAT;//�����������ʼλ��
    std::vector<assistx2::Stream> hu_message_;
};

ZhuanZhuanRoom::ZhuanZhuanRoom(std::uint32_t id, std::string type):
 PrivateRoom(id,type),
 pImpl_(new ZhuanZhuanRoomImpl(this))
{
    pImpl_->card_logic_ = std::make_shared<ZZMajiangLogic>();
}

ZhuanZhuanRoom::~ZhuanZhuanRoom()
{
    
}

void ZhuanZhuanRoom::set_zhama_num(const std::int32_t value)
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

void ZhuanZhuanRoom::set_operation(const std::int32_t value)
{
    pImpl_->room_operation_ = value;
}

void ZhuanZhuanRoom::set_playtype(const std::int32_t value)
{
    pImpl_->play_type_ = value;
}

void ZhuanZhuanRoom::set_playlogic(const std::int32_t value)
{
    if (value == 2)
    {
        pImpl_->card_logic_ = std::make_shared<ZZMajiangLogicTwoPlayer>();
    }
    else
    {
        pImpl_->card_logic_ = std::make_shared<ZZMajiangLogic>();
    }
}

std::int32_t ZhuanZhuanRoom::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto cmd = packet->GetCmd();
    DLOG(INFO) << "ZhuanZhuanRoom::OnMessage()->cmd:" << cmd << " roomid:=" << scene_id()
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

void ZhuanZhuanRoom::OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context)
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

void ZhuanZhuanRoom::OnGameStart()
{
    PrivateRoom::OnGameStart();

    if (table_obj()->GetSeats().size() == 2u)
    {
        card_generator()->Reset(CardGenerator::Type::ZZ_TWO_PLAYER);
    }
    else if ((pImpl_->room_operation_ & 0x04) == 0x04)
    {
        card_generator()->Reset(CardGenerator::Type::HZ_MAJIANG);
    }
    else
    {
        card_generator()->Reset(CardGenerator::Type::GENERAL_MAJIANG);
    }
    
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

    pImpl_->CheckQSHu(banker_seatno(), operations);

    if (operations.size() == 0)
    {
        operations.push_back(CardLogic::PLAY_OPERA);
    }

    pImpl_->NotifyOperation(banker_seatno(), operations);
}

void ZhuanZhuanRoom::OnGameOver()
{
    DLOG(INFO) << "OnGameOver: roomid:=" << scene_id()
        << ",type:=" << static_cast<std::int32_t>(pImpl_->hu_type_);

    auto now_banker_seatno = banker_seatno();
    DCHECK(now_banker_seatno != Table::INVALID_SEAT);

    if (pImpl_->hu_type_ == PrivateRoom::HuType::ZIMOHU)
    {
        pImpl_->ZhaNiao();
        DCHECK(pImpl_->hu_players_.size() > 0u);
        auto hu_seatno = pImpl_->hu_players_[0];
        DCHECK(hu_seatno != Table::INVALID_SEAT);
        auto num = pImpl_->GetZhongNiaoNum(hu_seatno);
        auto sum_score = 2 + num;
        pImpl_->CalculateScore(hu_seatno, sum_score);
        if (pImpl_->IsZhuangXianCalculateScore())
        {
            if (hu_seatno == now_banker_seatno)
            {
                pImpl_->CalculateScore(hu_seatno, 1);
            }
            else
            {
                auto now_banker_seat = table_obj()->GetBySeatNo(now_banker_seatno);
                auto hu_seat = table_obj()->GetBySeatNo(hu_seatno);
                now_banker_seat->data()->game_score_ -= 1;
                hu_seat->data()->game_score_ += 1;
            }
        }
        set_banker_seatno( hu_seatno );
    }
    else if (pImpl_->hu_type_ == PrivateRoom::HuType::QINAGGANGHU ||
        pImpl_->hu_type_ == PrivateRoom::HuType::DIANPAOHU)
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
            auto num = pImpl_->GetZhongNiaoNum(iter);
            auto sum_score = 1 + num;

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
        //����������
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
        //���ϵ���
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

std::int32_t ZhuanZhuanRoom::Disband()
{
    pImpl_->ClearRoomData();

    return PrivateRoom::Disband();
}

void ZhuanZhuanRoom::OnDisbandRoom(DisbandType type)
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

void ZhuanZhuanRoom::OnReConect(std::shared_ptr<Agent > player)
{
    //˳��̶�,���ܸı�
    auto state = room_state();

    PrivateRoom::OnReConect(player);

    if (state != RoomBase::RoomState::PLAYING)
    {
        return;
    }

    pImpl_->NotifyTableData(player);
}

std::string ZhuanZhuanRoom::RoomDataToString(bool isContainTableData)
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
    root.push_back(json_spirit::Pair("playtype", pImpl_->play_type_));
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

void ZhuanZhuanRoom::StringToRoomData(const std::string& str)
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
        if (iter.name_ == "playtype")
        {
            pImpl_->play_type_ = assistx2::ToInt32(iter.value_);
        }
    }

    PrivateRoom::StringToRoomData(str);

    set_playlogic(table_obj()->GetSeats().size());
}

ZhuanZhuanRoomImpl::ZhuanZhuanRoomImpl(ZhuanZhuanRoom* owner) :
    owner_(owner)
{

}

ZhuanZhuanRoomImpl::~ZhuanZhuanRoomImpl()
{
}

void ZhuanZhuanRoomImpl::BroadCastNextPlayer(std::int32_t seatno)
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
}

void ZhuanZhuanRoomImpl::NotifyOperation(std::int32_t seatno,
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
    seat->data()->operation_id_ = ss.str();

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

void ZhuanZhuanRoomImpl::OnPlay(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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
            << ",mid:=" << player->uid()<< ",card_name:=" << card->getName() << ",not in hand_cards";
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

void ZhuanZhuanRoomImpl::OnPeng(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    seat->data()->now_operate_.clear();
    seat->data()->is_qianggang_hu_ = false;
    RemoveHuPlayerOperation(seatno);

    if (hu_player_operation_.size() == 0)
    {
        DoPeng(seat,std::string(""));
    }
    else
    {
        next_func_ = std::bind(&ZhuanZhuanRoomImpl::DoPeng, this, seat, std::string(""));
        SendErrorCode(player, CLIENT_REQUEST_ONPENG, -2);
    }
    
}

void ZhuanZhuanRoomImpl::OnGang(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    seat->data()->is_qianggang_hu_ = false;
    seat->data()->now_operate_.clear();
    RemoveHuPlayerOperation(seatno);

    if (hu_player_operation_.size() == 0)
    {
        DoGang(seat, card_name);
    }
    else
    {
        next_func_ = std::bind(&ZhuanZhuanRoomImpl::DoGang, this, seat, card_name);
        SendErrorCode(player, CLIENT_REQUEST_ONGANG, -2);
    }
}

void ZhuanZhuanRoomImpl::OnHu(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    next_func_ = nullptr;
    seat->data()->now_operate_.clear();

    if (now_played_seatno_ != Table::INVALID_SEAT &&
        seat->data()->mo_card_ == nullptr)
    {
        DoDianPaoHu(seat, std::string(""));
    }
    else
    {
        DoZiMoHu(seat, std::string(""));
    }
    
    GameDataManager::getInstance()->OnOperation(owner_, seatno, CardLogic::HUPAI_OPERA, hu_card_);

    hu_players_.push_back(seatno);

    RemoveHuPlayerOperation(seatno);

    auto &seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->now_operate_.clear();
    }

    if (hu_player_operation_.size() == 0u)
    {
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
    }
    else
    {
        for (auto iter : hu_player_operation_)
        {
            NotifyOperation(iter, std::vector<CardLogic::OperationType>(1, CardLogic::HUPAI_OPERA));
        }
    }
}
void ZhuanZhuanRoomImpl::OnCancle(std::shared_ptr<Agent > player, assistx2::Stream * packet)
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

    if (CheckOperation(seat,CardLogic::HUPAI_OPERA) == true)
    {
        SetGuoCard(seat);
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

    seat->data()->is_qianggang_hu_ = false;

    RemoveHuPlayerOperation(seatno);

    if (hu_player_operation_.size() == 0 && hu_players_.size() != 0)
    {
        owner_->NewTimer(1, SceneTimerContext::MAJIANG_GAMEOVER, nullptr);
        return;
    }

    if (IsNoOperator() != true) return;

    if (hu_player_operation_.size() == 0 && next_func_ == nullptr )
    {
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
    else if (hu_player_operation_.size() != 0)
    {
        //do nothing
    }
    else
    {
        next_func_(seat,std::string("test"));
    }
}

bool ZhuanZhuanRoomImpl::CheckOperation(Seat* seat, CardLogic::OperationType operate)
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

std::vector<CardLogic::OperationType> ZhuanZhuanRoomImpl::GetOperations(std::int32_t seatno)
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

void ZhuanZhuanRoomImpl::BroadCastPlayedCard(std::int32_t seatno, std::shared_ptr<Card> card)
{
    assistx2::Stream stream(SERVER_BROADCAST_PLAYED_CARD);
    stream.Write(seatno);
    stream.Write(card->getName());
    stream.End();

    owner_->BroadCast(stream);
}

void ZhuanZhuanRoomImpl::FindOperatorPlayer(std::int32_t seatno, std::shared_ptr<Card> card)
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
        if (play_type_ == TYPE_DIANPAO_HU)
        {
            if (CanGang(card) == true &&
                CheckHu(card, iter) == true)
            {
                AddHuPlayerOperation(iter->seat_no());
                operatios.push_back(CardLogic::HUPAI_OPERA);
            }
            if (operatios.size() != 0u)
            {
                has_operator = true;
                NotifyOperation(iter->seat_no(), operatios);
            }
        }
        else
        {
            if (operatios.size() != 0u)
            {
                has_operator = true;
                NotifyOperation(iter->seat_no(), operatios);
                break;
            }
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

bool ZhuanZhuanRoomImpl::FindGongGang(Seat* seat, std::shared_ptr<Card> card)
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

bool ZhuanZhuanRoomImpl::FindQiangGangHu(std::int32_t seatno, std::shared_ptr<Card> card)
{
    if (play_type_ == TYPE_ZIMO_HU)
    {
        return false;
    }
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
            iter->data()->is_qianggang_hu_ = true;
            AddHuPlayerOperation(iter->seat_no());
            NotifyOperation(iter->seat_no(), std::vector<CardLogic::OperationType>(1, CardLogic::HUPAI_OPERA));
        }
    }

    if (hu_count > 0)
    {
        return true;
    }

    return false;
}

void ZhuanZhuanRoomImpl::MoCard(std::shared_ptr<Agent > player)
{
    auto seatno = player->seat_no();
    DCHECK(seatno != Table::INVALID_SEAT);

    auto seat = owner_->table_obj()->GetBySeatNo(seatno);
    DCHECK(seat != nullptr);

    auto cardcount = static_cast<std::int32_t>(
        owner_->card_generator()->count());
    if (cardcount == 0)
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

void ZhuanZhuanRoomImpl::SendErrorCode(std::shared_ptr<Agent > player,
    const std::int16_t cmd, const std::int32_t err)
{
    assistx2::Stream stream(cmd);
    stream.Write(err);
    stream.End();

    player->SendTo(stream);
}

void ZhuanZhuanRoomImpl::CalculateScore(std::int32_t seatno, std::int32_t score)
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

void ZhuanZhuanRoomImpl::ZhaNiao()
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
            auto num = iter->seat_no() - zhama_start_seatno_;//��Һͺ��ҵľ���
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

std::int32_t ZhuanZhuanRoomImpl::GetZhongNiaoNum(std::int32_t seatno)
{
    auto it = zhong_ma_seats_.find(seatno);
    if (it != zhong_ma_seats_.end())
    {
        return it->second.size();
    }

    return 0;
}

void ZhuanZhuanRoomImpl::ClearGameData()
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
    hu_player_operation_.clear();
    next_func_ = nullptr;
    hu_message_.clear();
    zhama_start_seatno_ = Table::INVALID_SEAT;
    hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->ClearNowGameData();
        auto state = ((iter->seat_player_state() & Seat::PLAYER_STATUS_NET_CLOSE) | Seat::PLAYER_STATUS_WAITING);
        iter->set_seat_player_state(state);
    }
}

void ZhuanZhuanRoomImpl::ClearRoomData()
{
    active_player_ = Table::INVALID_SEAT;
    now_operator_ = Table::INVALID_SEAT;
    now_played_seatno_ = Table::INVALID_SEAT;
    last_mo_seat_ = Table::INVALID_SEAT;
    beiqiang_seat_ = Table::INVALID_SEAT;
    zhama_num_ = 0;
    room_operation_ = 0;
    hu_players_.clear();
    zhong_ma_seats_.clear();
    hu_card_ = nullptr;
    vec_niao_.clear();
    hu_player_operation_.clear();
    next_func_ = nullptr;
    hu_message_.clear();
    zhama_start_seatno_ = Table::INVALID_SEAT;
    hu_type_ = PrivateRoom::HuType::CHOUZHUANG;
    auto& seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        iter->data()->ClearAllData();
        iter->set_seat_player_state(Seat::PLAYER_STATUS_WAITING);
    }
}

void ZhuanZhuanRoomImpl::NotifyTableData(std::shared_ptr<Agent > player)
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
    //���ϵ���
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
        //����������
        auto& operated_cards = iter->data()->operated_cards_;
        stream.Write(static_cast<std::int32_t>(operated_cards.size()));
        for (auto it : operated_cards)
        {
            stream.Write(it.first->getName());
            stream.Write(static_cast<std::int32_t>(it.second));
        }
        //����������
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

    if (seat->data()->now_operate_.size() != 0)
    {
        NotifyOperation(seatno, seat->data()->now_operate_);
    }
}

bool ZhuanZhuanRoomImpl::CheckHu(const std::shared_ptr<Card> card, Seat* seat)
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
    auto res = card_logic_->CheckHu(card, seat->data()->hand_cards_);
    if (((room_operation_ & 0x02) == 0x02) && res == false)
    {
        if (seat->data()->operated_cards_.size() != 0u)
        {
            return res;
        }
        res = card_logic_->CheckHu7Dui(card, seat->data()->hand_cards_);
    }
    
    return res;
}

void ZhuanZhuanRoomImpl::IncrMingtang(Seat* seat, MingTangType type)
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

void ZhuanZhuanRoomImpl::RoomAccount()
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

bool ZhuanZhuanRoomImpl::IsZhuangXianCalculateScore()
{
    if ((room_operation_ & 0x01) == 0x01)
    {
        return true;
    }
    return false;
}

bool ZhuanZhuanRoomImpl::CanGang(const std::shared_ptr<Card> card)
{
    if ((room_operation_ & 0x04) != 0x04)
    {
        return true;
    }

    if (card->getName() != "HZ")
    {
        return true;
    }

    return false;
}

void ZhuanZhuanRoomImpl::CheckQSHu(std::int32_t seatno, std::vector<CardLogic::OperationType>& operation)
{
    if ((room_operation_ & 0x04) != 0x04)
    {
        return;
    }
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

void ZhuanZhuanRoomImpl::AddHuPlayerOperation(const std::int32_t seatno)
{
    auto iter = std::find_if(hu_player_operation_.begin(), hu_player_operation_.end(), 
        [seatno](const std::int32_t value)
    {
        return value == seatno;
    });
    if (iter == hu_player_operation_.end())
    {
        hu_player_operation_.push_back(seatno);
    }
}

void ZhuanZhuanRoomImpl::RemoveHuPlayerOperation(const std::int32_t seatno)
{
    auto iter = std::find_if(hu_player_operation_.begin(), hu_player_operation_.end(),
        [seatno](const std::int32_t value)
    {
        return value == seatno;
    });
    if (iter != hu_player_operation_.end())
    {
        hu_player_operation_.erase(iter);
    }
}

void ZhuanZhuanRoomImpl::DoPeng(Seat* seat, std::string card_name)
{
    next_func_ = nullptr;

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

    NotifyOperation(seatno, std::vector<CardLogic::OperationType>(1, CardLogic::PLAY_OPERA));
}

void ZhuanZhuanRoomImpl::DoGang(Seat* seat, std::string card_name)
{
    next_func_ = nullptr;

    auto seatno = seat->seat_no();
    OperCardsType type = NULL_TYPE;
    std::shared_ptr<Card> card = nullptr;
    Seat* operaseat = nullptr;
    if (active_player_ == now_operator_)
    {
        operaseat = seat;
        auto card_recv = CardFactory::MakeMajiangCard(card_name);
        if (card_recv != nullptr &&
            seat->data()->hand_cards_->card_count(card_recv) == 4 &&
            CanGang(card_recv) == true)
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
                if (iter.num == 4 && 
                    CanGang(iter.card) == true)
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

        auto score = owner_->table_obj()->GetSeats().size() - 1;
        played_seat->data()->game_score_ -= score;
        seat->data()->game_score_ += score;

        seat->data()->gang_ming_num_ += 1;
        played_seat->data()->gang_fang_num_ += 1;

        type = MGANG;

        IncrMingtang(played_seat, MINGTANG_DIANGANG);
        IncrMingtang(seat, MINGTANG_JIEGANG);

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
            next_func_ = std::bind(&ZhuanZhuanRoomImpl::DoGongGang, this, seat, card->getName());
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
        seat->data()->gang_gong_num_ += 1;
        IncrMingtang(seat, MINGTANG_GONGGANG);
        CalculateScore(seatno, 1);
    }
    else if (type == AGANG)
    {
        seat->data()->gang_an_num_ += 1;
        CalculateScore(seatno, 2);
    }

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

    owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, seat);
}

void ZhuanZhuanRoomImpl::DoGongGang(Seat* seat, std::string card_name)
{
    next_func_ = nullptr;

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
    seat->data()->gang_gong_num_ += 1;
    IncrMingtang(seat, MINGTANG_GONGGANG);
    CalculateScore(seatno, 1);

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

    owner_->NewTimer(1, SceneTimerContext::MAJIANG_MOPAI, seat);
}

void ZhuanZhuanRoomImpl::DoZiMoHu(Seat* seat, std::string card_name)
{
    hu_type_ = PrivateRoom::HuType::ZIMOHU;

    auto seatno = seat->seat_no();
    seat->data()->zimo_num_ += 1;
    IncrMingtang(seat, MINGTANG_ZIMOHU);

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

    zhama_start_seatno_ = seatno;
}

void ZhuanZhuanRoomImpl::DoDianPaoHu(Seat* seat, std::string card_name)
{
    Seat* dianpaoseat = nullptr;
    if (seat->data()->is_qianggang_hu_ == false)
    {
        dianpaoseat = owner_->table_obj()->GetBySeatNo(now_played_seatno_);
        DCHECK(dianpaoseat != nullptr);
        hu_card_ = dianpaoseat->data()->played_cards_.top();
        //dianpaoseat->data()->played_cards_.pop();
        hu_type_ = PrivateRoom::HuType::DIANPAOHU;
        beiqiang_seat_ = now_played_seatno_;
    }
    else
    {
        dianpaoseat = owner_->table_obj()->GetBySeatNo(active_player_);
        DCHECK(dianpaoseat != nullptr);
        hu_card_ = dianpaoseat->data()->mo_card_;
        DCHECK(hu_card_ != nullptr);
        hu_type_ = PrivateRoom::HuType::QINAGGANGHU;
        beiqiang_seat_ = active_player_;
    }

    dianpaoseat->data()->beiqiang_hu_num_ += 1;
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

    seat->data()->gang_hu_num_ += 1;
    IncrMingtang(seat, MINGTANG_JIEPAO);
}

void ZhuanZhuanRoomImpl::AddZhongMaSeats(const std::int32_t seatno, const std::shared_ptr<Card> card)
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

bool ZhuanZhuanRoomImpl::IsNoOperator()
{
    auto seats = owner_->table_obj()->GetSeats();
    for (auto iter : seats)
    {
        if (iter->data()->now_operate_.size() != 0)
        {
            return false;
        }
    }

    return true;
}

void ZhuanZhuanRoomImpl::SetGuoCard(Seat* seat)
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