#ifndef FRAMEWORK_GAME_CHANGSHAROOM_H_
#define FRAMEWORK_GAME_CHANGSHAROOM_H_

#include "privateroom.h"

class ChangShaRoomImpl;
class ChangShaRoom :public PrivateRoom
{
public:
    ChangShaRoom(std::uint32_t id, std::string type);
    virtual ~ChangShaRoom();

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    virtual std::int32_t Disband();

    virtual std::string RoomDataToString(bool isContainTableData);
    virtual void StringToRoomData(const std::string& str);

    void set_zhama_num(const std::int32_t value);
    void set_operation(const std::int32_t value);
protected:
    virtual void OnGameStart();
    virtual void OnGameOver();
    virtual void OnDisbandRoom(DisbandType type);
    virtual void OnReConect(std::shared_ptr<Agent > player);
    virtual void OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context);
private:
    friend class ChangShaRoomImpl;
    std::unique_ptr< ChangShaRoomImpl > pImpl_;
};

#endif