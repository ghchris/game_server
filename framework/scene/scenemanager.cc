#include "scenemanager.h"
#include "GameHall.h"

class SceneManagerImpl
{
public:
    SceneManagerImpl();
    ~SceneManagerImpl();
public:
    GameHall* game_hall_;
};

SceneManager::SceneManager():
 pImpl_(new SceneManagerImpl)
{

}

SceneManager::~SceneManager()
{

}

bool SceneManager::Initialize()
{
    pImpl_->game_hall_ = new GameHall;

    return true;
}

Scene* SceneManager::default_scene()
{
    return pImpl_->game_hall_;
}

SceneManagerImpl::SceneManagerImpl()
{
}

SceneManagerImpl::~SceneManagerImpl()
{
}