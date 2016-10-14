#ifndef FRAMEWORK_GAME_PRIVATEROOM_H_
#define FRAMEWORK_GAME_PRIVATEROOM_H_

#include "roombase.h"

class CardGenerator;
class PrivateRoomImpl;
class PrivateRoom:public RoomBase
{
public:
    PrivateRoom(std::uint32_t seat_count, std::uint32_t id, std::string type);
    virtual ~PrivateRoom();

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);

    virtual std::int32_t Enter(std::shared_ptr<Agent > player);
    virtual std::int32_t Leave(std::shared_ptr<Agent > player);
protected:
    std::shared_ptr<CardGenerator> card_generator();
protected:
    virtual void OnGameStart();
    virtual void OnGameOver();
    virtual void OnReConect(std::shared_ptr<Agent > player);
private:
    friend class PrivateRoomImpl;
    std::unique_ptr< PrivateRoomImpl > pImpl_;
};

#endif //FRAMEWORK_GAME_PRIVATEROOM_H_
