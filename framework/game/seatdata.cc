#include "seatdata.h"

void Data::ClearAllData()
{
    gang_an_num_ = 0; //���ܴ���
    gang_ming_num_ = 0;//����or�Ӹܴ���
    gang_fang_num_ = 0;//�Ÿܴ���
    gang_gong_num_ = 0;//���ܴ���
    gang_hu_num_ = 0;//���ܺ�����
    beiqiang_hu_num_ = 0;//�����ܺ��Ĵ���
    zimo_num_ = 0;//�����Ĵ���
    seat_score_ = 0;
    ClearNowGameData();
}

void Data::ClearNowGameData()
{
    game_score_ = 0;
    hand_cards_ = nullptr;
    mo_card_ = nullptr;
    now_operate_.clear();
    operated_cards_.clear();
    while (!played_cards_.empty())
    {
        played_cards_.pop();
    }
}

void Data::set_json_to_seat_data(std::string json)
{

}

std::string Data::seat_json_data()
{
    return "";
}