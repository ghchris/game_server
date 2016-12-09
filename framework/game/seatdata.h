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
    PENG = 0,       //��
    MGANG = 1,  //����
    AGANG = 2,   //����
    GGANG = 3,   //����
};

enum MingTangType
{
    MINGTANG_ZIMOHU = 0,                 //������
    MINGTANG_WUHONGZHONG = 1, //�޺���
    MINGTANG_DIANGANG = 2,           //���
    MINGTANG_JIEGANG = 3,               //�Ӹ�
    MINGTANG_ANGANG = 4,             //����
    MINGTANG_GONGGANG = 5,       //����
    MINGTANG_DIANPAO = 6,           //����
    MINGTANG_JIEPAO = 7                //����
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
public://��������
    std::int64_t seat_score_ = 0; //��λ����
    std::int32_t gang_an_num_ = 0; //���ܴ���
    std::int32_t gang_ming_num_ = 0;//����or�Ӹܴ���
    std::int32_t gang_fang_num_ = 0;//�Ÿܴ���
    std::int32_t gang_gong_num_ = 0;//���ܴ���
    std::int32_t gang_hu_num_ = 0;//���ܺ�����
    std::int32_t beiqiang_hu_num_ = 0;//�����ܺ��Ĵ���
    std::int32_t zimo_num_ = 0;//�����Ĵ���
public://������Ϸ����
    std::int32_t game_score_ = 0;//���ֻ���
    std::shared_ptr<CardGroup> hand_cards_;                     //����
    std::stack<std::shared_ptr< Card > > played_cards_;      //�������
    std::shared_ptr< Card > mo_card_ = nullptr;                  //������
    std::vector<CardLogic::OperationType> now_operate_;//��ǰ�Ĳ���
    std::map<std::shared_ptr< Card >, OperCardsType> operated_cards_;     //�Ѳ�������
    std::map<MingTangType, std::int32_t> mingtang_types_;
    bool is_qianggang_hu_ = false;
public:
    time_t start_playing_time_ = 0; //��Ϸ��ʼʱ��
    time_t end_playing_time_ = 0;  //��Ϸ����ʱ��
    std::int32_t disband_vote_ = 0; //��ɢͶƱ
};

#endif