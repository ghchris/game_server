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
    PENG = 0,       //��
    MGANG = 1,  //����
    AGANG = 2,   //����
    GGANG = 3,   //����
    CHI = 4,         //����
    MBU = 5,      //������
    ABU = 6,       //����
    GBU = 7,       //����
};

enum MingTangType
{
    MINGTANG_NULL = -1,
    MINGTANG_ZIMOHU = 0,                 //������
    MINGTANG_WUHONGZHONG = 1, //�޺���
    MINGTANG_DIANGANG = 2,           //���
    MINGTANG_JIEGANG = 3,               //�Ӹ�
    MINGTANG_ANGANG = 4,             //����
    MINGTANG_GONGGANG = 5,       //����
    MINGTANG_DIANPAO = 6,           //����
    MINGTANG_JIEPAO = 7,                //����
    MINGTANG_PING_HU = 8,            //ƽ��
    MINGTANG_DA_SI_XI = 9,                   //����ϲ
    MINGTANG_BAN_BAN_HU = 10,          //����
    MINGTANG_QUE_YI_SE = 11,                //ȱһɫ
    MINGTANG_SIX_SIX_SHUN = 12,         //����˳
    MINGTANG_PENG_PENG_HU = 13,     //������
    MINGTANG_JIANG_JIANG_HU = 14,   //������
    MINGTANG_QING_YI_SE = 15,            //��һɫ
    MINGTANG_HAI_DI_LAO_YUE = 16,   //��������
    MINGTANG_HAI_DI_PAO = 17,          //������
    MINGTANG_QI_XIAODUI = 18,        //��С��
    MINGTANG_HAOHUA_QIXIAODUI = 19, //������С��
    MINGTANG_SHUANG_HAOHUA_QIXIAODUI = 20, //˫��7С��
    MINGTANG_GANGSHANG_KAIHUA = 21, //���Ͽ���
    MINGTANG_QIANGGANG_HU = 22,          //���ܺ�
    MINGTANG_GANGSHANG_PAO = 23,       //������
    MINGTANG_QUAN_QIU_REN = 24,           //ȫ����
    MINGTANG_TIAN_HU = 25,                       //���
    MINGTANG_DI_HU = 26                            //�غ�
};

enum HuCardType
{
    DA_HU = 0,
    XIAO_HU = 1,
};

enum MingTangOperationType
{
    SIXI_OPERA = 200,          //��ϲ����
    SIXSIXSHUN_OPERA = 201,        //����혲���
    BANBAN_OPERA = 202,   //��������
    QUEYISE_OPERA = 203,  //ȱһɫ����
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
public://��������
    std::int64_t seat_score_ = 0; //��λ����
    std::int32_t gang_an_num_ = 0; //���ܴ���
    std::int32_t gang_ming_num_ = 0;//����or�Ӹܴ���
    std::int32_t gang_fang_num_ = 0;//�Ÿܴ���
    std::int32_t gang_gong_num_ = 0;//���ܴ���
    std::int32_t gang_hu_num_ = 0;//���ܺ�����
    std::int32_t beiqiang_hu_num_ = 0;//�����ܺ��Ĵ���
    std::int32_t zimo_num_ = 0;//�����Ĵ���
//��ɳ�齫����
    std::int32_t dahu_zimo_ = 0;        //�������
    std::int32_t xiaohu_zimo_ = 0;      //С������
    std::int32_t dahu_dianpao_ = 0;     //�������
    std::int32_t xiaohu_dianpao_ = 0; //С������
    std::int32_t dahu_jiepao_ = 0;       //�������
    std::int32_t xiaohu_jiepao_ = 0;    //С������
public://������Ϸ����
    std::int32_t game_score_ = 0;//���ֻ���
    std::shared_ptr<CardGroup> hand_cards_;                     //����
    std::stack<std::shared_ptr< Card > > played_cards_;      //�������
    std::shared_ptr< Card > mo_card_ = nullptr;                  //������
    std::vector<CardLogic::OperationType> now_operate_;//��ǰ�Ĳ���
    std::map<std::shared_ptr< Card >, OperCardsType> operated_cards_;     //�Ѳ�������
    std::map<MingTangType, std::int32_t> mingtang_types_;
    bool is_qianggang_hu_ = false;
    std::string operation_id_;
    std::vector< std::shared_ptr<Card> > guo_cards_; //��������
public:
    time_t start_playing_time_ = 0; //��Ϸ��ʼʱ��
    time_t end_playing_time_ = 0;  //��Ϸ����ʱ��
    std::int32_t disband_vote_ = 0; //��ɢͶƱ
public:
    std::map<CardLogic::OperationType, OperationsInfo> cs_now_operate_;//��ɳ�齫��ǰ����
    bool is_ting = false;
    std::shared_ptr<Card> gang_card_ = nullptr;
    std::vector<std::pair<std::shared_ptr<Card>, std::map<CardLogic::OperationType, OperationsInfo>>> card_now_operates_;
    std::set<MingTangOperationType> now_mingtang_operate_;//��ǰ���õĲ���
    //std::map
};

#endif