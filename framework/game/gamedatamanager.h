#ifndef FRAMEWORK_GAME_GAMEDATAMANAGER_H_
#define FRAMEWORK_GAME_GAMEDATAMANAGER_H_

#include <assistx2/tcphandler_wrapper.h>
#include <assistx2/singleton.h>
#include <memory>
#include "cardlogic.h"
#include "define.h"
#include <map>
#include <set>
#include "card.h"
#include "seatdata.h"

class Card;
class RoomBase;
class WatchDog;
class GameDataManagerImpl;
class GameDataManager:public Singleton<GameDataManager>
{
public:
    enum
    {
        DATA_GAME_START = 0,							//ÓÎÏ·¿ªÊ¼
        DATA_PLAY,
        DATA_GAME_OVER,
        DATA_ZHAMA
    };
public:
    GameDataManager();
    virtual ~GameDataManager();
public:
    void Initialize(WatchDog* wtachdog);
    void OnGameStart(RoomBase* room);
    void OnOperation(RoomBase* room,std::int32_t seatno,
        CardLogic::OperationType operation, std::shared_ptr< Card >card, MingTangType type = MINGTANG_NULL);
    void OnGameOver(RoomBase* room);
    void OnZhaMa(RoomBase* room,const Cards& card);
    void OnLogin(std::int32_t gp,uid_type mid);
    void OnLogout(std::int32_t gp, uid_type mid);
    void ResetRoomData(RoomBase* room);
    bool UpdataRoomData(RoomBase* room);
    void SaveCmdStream(uid_type mid,const assistx2::Stream& stream);
    void DeleteCmdStream(uid_type mid, const std::int16_t cmd);
    const std::vector<std::pair<std::int16_t, assistx2::Stream>> GetCmdStream(uid_type mid) const;
public:
    const std::map<std::int32_t, std::set<uid_type> >& login_players() const;
    const std::map<std::int32_t, std::set<uid_type> >& playing_players() const;
    std::string GetRoomRecord(std::int32_t roomid);
private:
    std::unique_ptr< GameDataManagerImpl > pImpl_;

};

#endif
