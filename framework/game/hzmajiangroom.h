#ifndef FRAMEWORK_GAME_HZMAJIANGROOM_H_
#define FRAMEWORK_GAME_HZMAJIANGROOM_H_

#include "privateroom.h"

class HzMajiangRoomImpl;
class HzMajiangRoom :public PrivateRoom
{
public:
    HzMajiangRoom(std::uint32_t id, std::string type);
    virtual ~HzMajiangRoom();

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);
    void set_zhama_num(const std::int32_t value);
    void set_support_7dui(const bool value);
protected:
    virtual void OnGameStart();
    virtual void OnGameOver(HuType type);
    virtual void OnDisbandRoom();
    virtual void OnReConect(std::shared_ptr<Agent > player);
private:
    friend class HzMajiangRoomImpl;
    std::unique_ptr< HzMajiangRoomImpl > pImpl_;
};

#endif