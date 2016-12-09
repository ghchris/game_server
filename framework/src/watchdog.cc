#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "watchdog.h"
#include <assistx2/tcphandler_wrapper.h>
#include <assistx2/stream2.h>
#include <assistx2/configure.h>
#include <assistx2/string_wrapper.h>
#include <assistx2/time_tracer.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "configmgr.h"
#include "playeragent.h"
#include "scenemanager.h"
#include "robotmanager.h"
#include "timerhelper.h"
#include "logserver.h"
#include "memberfides.pb.h"
#include "roombase.h"
#include "gamedatamanager.h"

//用户登录
const static std::int16_t CLIENT_REQUEST_LOGIN = 1000;
const static std::int16_t SERVER_VERSION_MESSAGE = 9999;
const static std::int16_t NET_CONNECT_CLOSED = 1999;

bool g_server_closed = false;
bool g_server_stopped = false;

class WatchDogImpl
{
public:
    WatchDogImpl(WatchDog* owner,boost::asio::io_service & ios);
    ~WatchDogImpl();

    std::int32_t OnMessage(assistx2::TcpHandler * socket, 
        boost::shared_ptr<assistx2::NativeStream > native_stream);
    std::int32_t OnConnect(assistx2::TcpHandler * handler, assistx2::ErrorCode err);
    std::int32_t OnClose(assistx2::TcpHandler * handler, assistx2::ErrorCode err);
    void OnRegister(assistx2::Stream * packet);
    void OnLogin(assistx2::Stream * packet);
    void OnConncetClose(const assistx2::Stream& packet);
    void OnRouteMessage(assistx2::Stream * packet);
    void OnAdminOperation(std::int32_t op);
    void ReSendCmdStream(uid_type mid);
    bool UpdataRoomData(Scene* scene);
public:
    void CloseServer();
public:
    WatchDog* owner_;
    boost::asio::io_service& io_service_;
    std::string run_id_;
    std::shared_ptr<assistx2::TcpHanlderWrapper> gateway_connector_;
    std::map<uid_type, std::shared_ptr<Agent>> players_agent_;
};

WatchDog::WatchDog(boost::asio::io_service & ios):
 pImpl_(new WatchDogImpl(this,ios))
{
    
}
WatchDog::~WatchDog()
{

}

bool WatchDog::Initialize()
{
    boost::uuids::random_generator  generator;

    std::stringstream ss;
    ss << generator();

    pImpl_->run_id_ = ss.str();

    GlobalTimerProxy::getInstance()->Init(pImpl_->io_service_);

    auto reader = ConfigMgr::getInstance()->app_config_obj();

    std::string host;
    std::string port;
    reader->getConfig("xGateWay", "host", host);
    reader->getConfig("xGateWay", "port", port);

    pImpl_->gateway_connector_ = std::make_shared<assistx2::TcpHanlderWrapper>(pImpl_->io_service_);
    if (pImpl_->gateway_connector_ == nullptr)
    {
        LOG(ERROR) << "WatchDogImpl::pImpl_->gateway_connector_ == nil";
        return false;
    }

    pImpl_->gateway_connector_->RegisterCloseHandler(std::bind(&WatchDogImpl::OnClose, 
        pImpl_.get(), std::placeholders::_1, std::placeholders::_2));
    pImpl_->gateway_connector_->RegisterConnectHandler(std::bind(&WatchDogImpl::OnConnect, 
        pImpl_.get(), std::placeholders::_1, std::placeholders::_2));
    pImpl_->gateway_connector_->RegisterMessageHandler(std::bind(&WatchDogImpl::OnMessage, 
        pImpl_.get(), std::placeholders::_1, std::placeholders::_2));
    pImpl_->gateway_connector_->Connect(host, static_cast<unsigned short>(assistx2::atoi_s(port)));

    GameDataManager::getInstance()->Initialize(this);
   
    SceneManager::getInstance()->Initialize(pImpl_->io_service_,this);

    RobotManager::getInstance()->Initialize(this);

    LogServer::getInstance()->Initialize(pImpl_->gateway_connector_);

    return true;
}

void WatchDog::RemoveAgent(uid_type uid)
{
    auto iter = pImpl_->players_agent_.find(uid);
    if (iter != pImpl_->players_agent_.end())
    {
        pImpl_->players_agent_.erase(iter);
    }
}

std::shared_ptr<Agent> WatchDog::NewAgent(uid_type uid)
{
    std::shared_ptr<Agent> player = nullptr;

    auto iter = pImpl_->players_agent_.find(uid);
    if (iter != pImpl_->players_agent_.end())
    {
        player = iter->second;
    }
    else
    {
        player = std::make_shared<PlayerAgent>(pImpl_->gateway_connector_);
        player->set_uid(uid);
        if (!player->Serialize(true))
        {
            return nullptr;
        }
        pImpl_->players_agent_.insert(std::make_pair(uid, player));
    }
    player->set_watch_dog(this);
    player->set_connect_status(false);
    player->set_scene_object(nullptr);

    return player;
}

std::shared_ptr<Agent> WatchDog::GetAgentByID(uid_type uid)
{
    auto iter = pImpl_->players_agent_.find(uid);
    if (iter != pImpl_->players_agent_.end())
    {
        return iter->second;
    }

    return nullptr;
}

WatchDogImpl::WatchDogImpl(WatchDog* owner, boost::asio::io_service & ios):
 owner_(owner),
 io_service_(ios)
{
}

WatchDogImpl::~WatchDogImpl()
{
}

std::int32_t WatchDogImpl::OnMessage(assistx2::TcpHandler * socket,
    boost::shared_ptr<assistx2::NativeStream > native_stream)
{
    assistx2::Stream packet(native_stream);
    const std::int32_t cmd = packet.GetCmd(); 

    std::stringstream ss;
    ss << "WatchDogImpl::OnMessage:-->" << cmd;

    assistx2::TimeTracer tracerline(new assistx2::SimpleDumpHelper((ss.str()), assistx2::TimeTracer::MILLISECOND));
  
    switch (cmd)
    {
    case SERVER_RESPONSE_REGISTER:
        OnRegister(&packet);
        return 0;
    case CLIENT_REQUEST_LOGIN:
        OnLogin(&packet);
        return 0;
    case STANDARD_ROUTE_PACKET:
        OnRouteMessage(&packet);
        return 0;
    case NET_CONNECT_CLOSED:
        OnConncetClose(packet);
        break;
    default:
        break;
    }

    auto uid = packet.Read<std::int32_t>();
    auto iter = players_agent_.find(uid);
    if (iter != players_agent_.end())
    {
        iter->second->Process(&packet);
    }

    return 0;
}

std::int32_t WatchDogImpl::OnConnect(assistx2::TcpHandler * handler, assistx2::ErrorCode err)
{
    std::int32_t error = handler->SetReceiveBufferSize(1024 * 128);
    if (error != 0)
    {
        LOG(ERROR) << "OnConnect, SET RECV BUFFER FAILED, err:=" << error;
    }

    error = handler->SetSendBufferSize(1024 * 128);
    if (error != 0)
    {
        LOG(ERROR) << "OnConnect, SET SEND BUFFER FAILED, err:=" << error;
    }

    auto server_id = ConfigMgr::getInstance()->server_id();

    assistx2::Stream stream(CLINET_REQUEST_REGISTER);
    stream.Write(SESSION_TYPE_GAME_SERVER);
    stream.Write(server_id);
    stream.Write(run_id_);
    stream.End();

    handler->Send(stream.GetNativeStream());

    return 0;
}

std::int32_t WatchDogImpl::OnClose(assistx2::TcpHandler * handler, assistx2::ErrorCode err)
{
    auto clone(players_agent_);
    for (auto it = clone.begin(); it != clone.end(); ++it)
    {
        if (it->second->scene_object() != nullptr)
        {
            
        }
    }

    if (g_server_closed == true)
    {
        io_service_.stop();
    }

    return 0;
}

void WatchDogImpl::OnRegister(assistx2::Stream * packet)
{
    const boost::int32_t err = packet->Read<boost::int32_t>();
    if (err != 0)
    {
        LOG(ERROR) << "RunFastGameMgr::OnMessage.  REGISTER FAILED. err:=" << err;
        io_service_.stop();
    }
}

void WatchDogImpl::OnLogin(assistx2::Stream * packet)
{
    const uid_type mid = packet->Read<std::int32_t>();
    const std::int32_t login_source = packet->Read<std::int32_t>();
    const std::int32_t game_session = packet->Read<std::int32_t>();
    const std::string ip_addr = packet->Read<std::string>();

    DLOG(INFO) << "WatchDogImpl::OnLogin mid:=" << mid << " login_source:=" 
        << login_source << " game_session:=" << game_session << " ip_addr:=" << ip_addr;

    auto now_scene = SceneManager::getInstance()->default_scene();
    std::shared_ptr<Agent> player = nullptr;

    auto iter = players_agent_.find(mid);
    if (iter != players_agent_.end())
    {
        player = iter->second;
    }
    else
    {
        player = std::make_shared<PlayerAgent>(gateway_connector_);
        player->set_uid(mid);
        if ( !player->Serialize(true) )
        {
            return;
        }
        players_agent_.insert(std::make_pair(mid, player));
    }
    player->set_ip_addr(ip_addr);
    player->set_game_session(game_session);
    player->set_watch_dog(owner_);
    player->set_connect_status(true);

    auto scene = player->scene_object();
    if (scene != nullptr && (UpdataRoomData(scene) == true))
    {
        now_scene = scene;
    }

    //发送服务器版本号
    assistx2::Stream stream(SERVER_VERSION_MESSAGE);
    stream.Write(ConfigMgr::getInstance()->server_version());
    stream.End();
    player->SendTo(stream);

    ReSendCmdStream(mid);

    auto res = now_scene->Enter(player);
    if (res >= 0)
    {
        player->set_scene_object(now_scene);
    }
    else
    {
        LOG(ERROR) << "EnterScene Failed! mid:=" << mid << ",Scene ID:="
            << now_scene->scene_id() << ",Scene Type:=" << now_scene->scene_type()
            << ",res:=" << res;
        return;
    }

    GameDataManager::getInstance()->OnLogin(player->member_fides()->gp(),player->uid());
}

void WatchDogImpl::OnConncetClose(const assistx2::Stream& packet)
{
    assistx2::Stream clone(packet);
    auto uid = clone.Read<std::int32_t>();
    auto iter = players_agent_.find(uid);
    if (iter != players_agent_.end())
    {
        GameDataManager::getInstance()->OnLogout(iter->second->member_fides()->gp(),
            iter->second->uid());
    }
}

void WatchDogImpl::OnRouteMessage(assistx2::Stream* packet)
{
    const short subcmd = packet->Read<std::int16_t>();
    switch (subcmd)
    {
    case SYSTEM_ADMINI_CMD:
    {
        const std::int32_t op = packet->Read<std::int32_t>();
        LOG(INFO) << "WatchDogImpl::OnRouteMessage. op:=" << op;

        OnAdminOperation(op);
    }
        break;
    default:
        break;
    }
}

void WatchDogImpl::OnAdminOperation(std::int32_t op)
{
    const std::int32_t ADMINI_CMD_STOP_SERVER = 2;

    if (op == ADMINI_CMD_STOP_SERVER)
    {
        g_server_stopped = true;
        GlobalTimerProxy::getInstance()->NewTimer(
            std::bind(&WatchDogImpl::CloseServer, this), 180);
    }
}

void WatchDogImpl::CloseServer()
{
    io_service_.stop();
}

void WatchDogImpl::ReSendCmdStream(uid_type mid)
{
    auto& vcCmdStream = GameDataManager::getInstance()->GetCmdStream(mid);
    for (auto iter : vcCmdStream)
    {
        gateway_connector_->SendTo(iter.second.GetNativeStream());
    }
}

bool WatchDogImpl::UpdataRoomData(Scene* scene)
{
    if (scene->scene_id() != 0)
    {
        auto room = dynamic_cast<RoomBase*>(scene);
        return GameDataManager::getInstance()->UpdataRoomData(room);
    }

    return true;
}