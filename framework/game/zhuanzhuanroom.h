#ifndef FRAMEWORK_GAME_ZHUANZHUANROOM_H_
#define FRAMEWORK_GAME_ZHUANZHUANROOM_H_

#include "privateroom.h"

class ZhuanZhuanRoomImpl;
class ZhuanZhuanRoom :public PrivateRoom
{
public:
    ZhuanZhuanRoom(std::uint32_t id, std::string type);
    virtual ~ZhuanZhuanRoom();

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    virtual std::int32_t Disband();

    virtual std::string RoomDataToString(bool isContainTableData);
    virtual void StringToRoomData(const std::string& str);

    void set_zhama_num(const std::int32_t value);
    void set_operation(const std::int32_t value);
    void set_playtype(const std::int32_t value);
    void set_playlogic(const std::int32_t value);
protected:
    virtual void OnGameStart();
    virtual void OnGameOver();
    virtual void OnDisbandRoom(DisbandType type);
    virtual void OnReConect(std::shared_ptr<Agent > player);
    virtual void OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context);
private:
    friend class ZhuanZhuanRoomImpl;
    std::unique_ptr< ZhuanZhuanRoomImpl > pImpl_;
};

#endif