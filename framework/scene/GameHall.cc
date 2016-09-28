#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <assistx2/tcphandler_wrapper.h>
#include "watchdog.h"
#include "playeragent.h"
#include "GameHall.h"

const static std::int16_t NET_CONNECT_CLOSED = 1999;

class GameHallImpl
{
public:
    GameHallImpl(GameHall* owner);
    ~GameHallImpl();
    void OnClientClose(std::shared_ptr<Agent > player);
public:
    GameHall* owner_;
};

GameHall::GameHall():
 SceneBase(0,"GH"),
 pImpl_(new GameHallImpl(this))
{

}

GameHall::~GameHall()
{

}


std::int32_t GameHall::OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet)
{
    auto cmd = packet->GetCmd();
    DLOG(INFO) << "GameHall::OnMessage()->cmd:" << cmd << " Scene:=" << scene_id()
        << " Scene Type:=" << scene_type();

    switch (cmd)
    {
    case NET_CONNECT_CLOSED:
        pImpl_->OnClientClose(player);
        return 0;
    default:
        break;
    }

    return 0;
}

GameHallImpl::GameHallImpl(GameHall* owner):
 owner_(owner)
{

}

GameHallImpl::~GameHallImpl()
{
}

void GameHallImpl::OnClientClose(std::shared_ptr<Agent > player)
{
    owner_->Leave(player);
    player->set_scene_object(nullptr);
    player->set_connect_status(false);
    player->watch_dog()->RemoveAgent(player->uid());
}