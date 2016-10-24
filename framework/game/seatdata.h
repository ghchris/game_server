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

class CardGroup;
class Data
{
public:
    Data() {}
    ~Data() {}

    void set_json_to_seat_data(std::string json);
    std::string seat_json_data();
   
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
};

#endif