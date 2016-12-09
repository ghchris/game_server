#ifndef FRAMEWORK_SCENE_ROOM_H_
#define FRAMEWORK_SCENE_ROOM_H_

#include "scenebase.h"
#include <memory>

class RoomData;
class Table;
class RoomBaseImpl;
class RoomBase :public SceneBase
{
public:
    enum class RoomState {CLOSED,WAITING,PLAYING,FREEZING};
public:
    RoomBase(std::uint32_t id, std::string type);
    virtual ~RoomBase();

    virtual std::int32_t Enter(std::shared_ptr<Agent > player);
    virtual std::int32_t Leave(std::shared_ptr<Agent > player);
    virtual std::int32_t Disband();

    virtual std::string RoomDataToString(bool isContainTableData);
    virtual void StringToRoomData(const std::string& str);

    void set_room_state(const RoomState& state);
    const RoomState& room_state() const;

    void set_room_owner(uid_type mid);
    const uid_type room_owner() const;

    void set_room_config_data(std::shared_ptr<RoomData> data);
    const std::shared_ptr<RoomData> room_conifg_data() const;

    void set_table_obj(std::shared_ptr<Table> obj);
    std::shared_ptr<Table> table_obj();

    void set_create_time(const time_t time);
    const time_t create_time() const;
private:
    std::unique_ptr< RoomBaseImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_ROOM_H_