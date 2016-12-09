#include "seatdata.h"
#include <json_spirit_reader_template.h>
#include <json_spirit_writer_template.h>
#include <assistx2/json_wrapper.h>
#include "common.h"

void Data::ClearAllData()
{
    gang_an_num_ = 0; //暗杠次数
    gang_ming_num_ = 0;//明杠or接杠次数
    gang_fang_num_ = 0;//放杠次数
    gang_gong_num_ = 0;//公杠次数
    gang_hu_num_ = 0;//抢杠胡次数
    beiqiang_hu_num_ = 0;//被抢杠胡的次数
    zimo_num_ = 0;//自摸的次数
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
    mingtang_types_.clear();
    is_qianggang_hu_ = false;
    while (!played_cards_.empty())
    {
        played_cards_.pop();
    }
}

void Data::set_string_to_seat_data(std::string json)
{
    auto value =  Common::StringToJson(json);
    auto & obj = value.get_obj();
    for (auto iter : obj)
    {
        if (iter.name_ == "gang_an_num")
        {
            gang_an_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "gang_ming_num")
        {
            gang_ming_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "gang_fang_num")
        {
            gang_fang_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "gang_gong_num")
        {
            gang_gong_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "gang_hu_num")
        {
            gang_hu_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "beiqiang_hu_num")
        {
            beiqiang_hu_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "zimo_num")
        {
            zimo_num_ = assistx2::ToInt32(iter.value_);
            continue;
        }
        if (iter.name_ == "seat_score")
        {
            seat_score_ = assistx2::ToInt32(iter.value_);
            continue;
        }
    }
}

std::string Data::seat_string_data()
{
    json_spirit::Object root;
    root.push_back(json_spirit::Pair("gang_an_num", gang_an_num_));
    root.push_back(json_spirit::Pair("gang_ming_num", gang_ming_num_));
    root.push_back(json_spirit::Pair("gang_fang_num", gang_fang_num_));
    root.push_back(json_spirit::Pair("gang_gong_num", gang_gong_num_));
    root.push_back(json_spirit::Pair("gang_hu_num", gang_hu_num_));
    root.push_back(json_spirit::Pair("beiqiang_hu_num", beiqiang_hu_num_));
    root.push_back(json_spirit::Pair("zimo_num", zimo_num_));
    root.push_back(json_spirit::Pair("seat_score", seat_score_));

    return json_spirit::write_string(json_spirit::Value(root));
}