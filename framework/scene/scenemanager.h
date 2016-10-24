#ifndef FRAMEWORK_SCENE_SCENEMANAGER_H_
#define FRAMEWORK_SCENE_SCENEMANAGER_H_

#include "scenebase.h"
#include <memory>
#include <assistx2/singleton.h>

class RoomBase;
class SceneManagerImpl;
class SceneManager:public Singleton<SceneManager >
{
public:
    SceneManager();
    virtual ~SceneManager();

    bool Initialize(boost::asio::io_service & ios);

    Scene* default_scene();
    RoomBase* GetRoomByType(const std::string& type,std::int32_t ju);
    void AttachActivedPrivateRoom(RoomBase* room);
    void DetachActivedPrivateRoom(RoomBase* room);
    RoomBase* GetRoomFromActivedPrivateRoom(const std::int32_t roomid);
private:
    std::unique_ptr< SceneManagerImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_SCENEMANAGER_H_