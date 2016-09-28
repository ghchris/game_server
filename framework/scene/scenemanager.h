#ifndef FRAMEWORK_SCENE_SCENEMANAGER_H_
#define FRAMEWORK_SCENE_SCENEMANAGER_H_

#include "scenebase.h"
#include <memory>

class SceneManagerImpl;
class SceneManager
{
public:
    SceneManager();
    virtual ~SceneManager();

    bool Initialize();

    Scene* default_scene();

private:
    std::unique_ptr< SceneManagerImpl > pImpl_;
};

#endif //FRAMEWORK_SCENE_SCENEMANAGER_H_