#ifndef FRAMEWORK_GAME_ROOMCONFIGDATA_H_
#define FRAMEWORK_GAME_ROOMCONFIGDATA_H_

#include <memory>
#include <vector>
#include <assistx2/configure.h>
#include <assistx2/singleton.h>

struct RoomData
{
    std::int32_t begin;
    std::int32_t end;
    std::int32_t ju;
    std::int32_t cost;
    std::string type;
    std::string name;
    std::int32_t serverid;
    std::int64_t taxation;
    time_t bettime;
    std::int32_t taxes_mode;
    std::int32_t sb;
    std::int64_t min;
    std::int64_t max;
};

class GameConfigDataImpl;
class GameConfigData:public Singleton<GameConfigData >
{
private:
    friend class DefaultBuilder<GameConfigData >;
    GameConfigData();
    virtual ~GameConfigData();
public:
    bool Init();
    const std::vector< std::shared_ptr<RoomData> >& rooms_data() const;
private:
    std::unique_ptr< GameConfigDataImpl > pImpl_;
};

#endif