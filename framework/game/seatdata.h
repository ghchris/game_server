#ifndef FRAMEWORK_GAME_SEATDATA_H_
#define FRAMEWORK_GAME_SEATDATA_H_

#include <stack>
#include <memory>
#include "card.h"
#include "cardlogic.h"
#include <map>
#include <set>

class Seat;
enum OperCardsType
{
    NULL_TYPE = -1,
    PENG = 0,       //碰
    MGANG = 1,  //明杠
    AGANG = 2,   //暗杠
    GGANG = 3,   //公杠
    CHI = 4,         //吃牌
    MBU = 5,      //明补张
    ABU = 6,       //暗补
    GBU = 7,       //公补
};

enum MingTangType
{
    MINGTANG_NULL = -1,
    MINGTANG_ZIMOHU = 0,                 //自摸胡
    MINGTANG_WUHONGZHONG = 1, //无红中
    MINGTANG_DIANGANG = 2,           //点杠
    MINGTANG_JIEGANG = 3,               //接杠
    MINGTANG_ANGANG = 4,             //暗杠
    MINGTANG_GONGGANG = 5,       //公杠
    MINGTANG_DIANPAO = 6,           //点炮
    MINGTANG_JIEPAO = 7,                //接炮
    MINGTANG_PING_HU = 8,            //平胡
    MINGTANG_DA_SI_XI = 9,                   //大四喜
    MINGTANG_BAN_BAN_HU = 10,          //板板胡
    MINGTANG_QUE_YI_SE = 11,                //缺一色
    MINGTANG_SIX_SIX_SHUN = 12,         //六六顺
    MINGTANG_PENG_PENG_HU = 13,     //碰碰胡
    MINGTANG_JIANG_JIANG_HU = 14,   //将将胡
    MINGTANG_QING_YI_SE = 15,            //清一色
    MINGTANG_HAI_DI_LAO_YUE = 16,   //海底捞月
    MINGTANG_HAI_DI_PAO = 17,          //海底炮
    MINGTANG_QI_XIAODUI = 18,        //七小对
    MINGTANG_HAOHUA_QIXIAODUI = 19, //豪华七小对
    MINGTANG_SHUANG_HAOHUA_QIXIAODUI = 20, //双豪7小队
    MINGTANG_GANGSHANG_KAIHUA = 21, //杠上开花
    MINGTANG_QIANGGANG_HU = 22,          //抢杠胡
    MINGTANG_GANGSHANG_PAO = 23,       //杠上炮
    MINGTANG_QUAN_QIU_REN = 24,           //全求人
    MINGTANG_TIAN_HU = 25,                       //天胡
    MINGTANG_DI_HU = 26                            //地胡
};

enum HuCardType
{
    DA_HU = 0,
    XIAO_HU = 1,
};

enum MingTangOperationType
{
    SIXI_OPERA = 200,          //四喜操作
    SIXSIXSHUN_OPERA = 201,        //六六順操作
    BANBAN_OPERA = 202,   //板板胡操作
    QUEYISE_OPERA = 203,  //缺一色操作
    CANCLE_OPERA = 204,
};

struct OperationsInfo
{
    std::function<void(Seat*, std::string)> cb_func_ = nullptr;
    std::string card;
};


class CardGroup;
class Data
{
public:
    Data() {}
    ~Data() {}

    void set_string_to_seat_data(std::string json);
    std::string seat_string_data();
    std::string cs_seat_string_data();
   
    void ClearAllData();
    void ClearNowGameData();
public://房间数据
    std::int64_t seat_score_ = 0; //座位积分
    std::int32_t gang_an_num_ = 0; //暗杠次数
    std::int32_t gang_ming_num_ = 0;//明杠or接杠次数
    std::int32_t gang_fang_num_ = 0;//放杠次数
    std::int32_t gang_gong_num_ = 0;//公杠次数
    std::int32_t gang_hu_num_ = 0;//抢杠胡次数
    std::int32_t beiqiang_hu_num_ = 0;//被抢杠胡的次数
    std::int32_t zimo_num_ = 0;//自摸的次数
//长沙麻将数据
    std::int32_t dahu_zimo_ = 0;        //大胡自摸
    std::int32_t xiaohu_zimo_ = 0;      //小胡自摸
    std::int32_t dahu_dianpao_ = 0;     //大胡点炮
    std::int32_t xiaohu_dianpao_ = 0; //小胡点炮
    std::int32_t dahu_jiepao_ = 0;       //大胡接炮
    std::int32_t xiaohu_jiepao_ = 0;    //小胡接炮
public://当局游戏数据
    std::int32_t game_score_ = 0;//当局积分
    std::shared_ptr<CardGroup> hand_cards_;                     //手牌
    std::stack<std::shared_ptr< Card > > played_cards_;      //打出的牌
    std::shared_ptr< Card > mo_card_ = nullptr;                  //摸的牌
    std::vector<CardLogic::OperationType> now_operate_;//当前的操作
    std::map<std::shared_ptr< Card >, OperCardsType> operated_cards_;     //已操作的牌
    std::map<MingTangType, std::int32_t> mingtang_types_;
    bool is_qianggang_hu_ = false;
    std::string operation_id_;
    std::vector< std::shared_ptr<Card> > guo_cards_; //过胡的牌
public:
    time_t start_playing_time_ = 0; //游戏开始时间
    time_t end_playing_time_ = 0;  //游戏结束时间
    std::int32_t disband_vote_ = 0; //解散投票
public:
    std::map<CardLogic::OperationType, OperationsInfo> cs_now_operate_;//长沙麻将当前操作
    bool is_ting = false;
    std::shared_ptr<Card> gang_card_ = nullptr;
    std::vector<std::pair<std::shared_ptr<Card>, std::map<CardLogic::OperationType, OperationsInfo>>> card_now_operates_;
    std::set<MingTangOperationType> now_mingtang_operate_;//当前名堂的操作
    //std::map
};

#endif