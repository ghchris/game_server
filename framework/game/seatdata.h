#ifndef FRAMEWORK_GAME_SEATDATA_H_
#define FRAMEWORK_GAME_SEATDATA_H_

#include <stack>
#include <memory>
#include "card.h"
#include "cardlogic.h"
#include <map>

enum OperCardsType
{
    NULL_TYPE = -1,
    PENG = 0,       //碰
    MGANG = 1,  //明杠
    AGANG = 2,   //暗杠
    GGANG = 3,   //公杠
};

enum MingTangType
{
    MINGTANG_ZIMOHU = 0,                 //自摸胡
    MINGTANG_WUHONGZHONG = 1, //无红中
    MINGTANG_DIANGANG = 2,           //点杠
    MINGTANG_JIEGANG = 3,               //接杠
    MINGTANG_ANGANG = 4,             //暗杠
    MINGTANG_GONGGANG = 5,       //公杠
    MINGTANG_DIANPAO = 6,           //点炮
    MINGTANG_JIEPAO = 7                //接炮
};

class CardGroup;
class Data
{
public:
    Data() {}
    ~Data() {}

    void set_string_to_seat_data(std::string json);
    std::string seat_string_data();
   
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
public://当局游戏数据
    std::int32_t game_score_ = 0;//当局积分
    std::shared_ptr<CardGroup> hand_cards_;                     //手牌
    std::stack<std::shared_ptr< Card > > played_cards_;      //打出的牌
    std::shared_ptr< Card > mo_card_ = nullptr;                  //摸的牌
    std::vector<CardLogic::OperationType> now_operate_;//当前的操作
    std::map<std::shared_ptr< Card >, OperCardsType> operated_cards_;     //已操作的牌
    std::map<MingTangType, std::int32_t> mingtang_types_;
    bool is_qianggang_hu_ = false;
public:
    time_t start_playing_time_ = 0; //游戏开始时间
    time_t end_playing_time_ = 0;  //游戏结束时间
    std::int32_t disband_vote_ = 0; //解散投票
};

#endif