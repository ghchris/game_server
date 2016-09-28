#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "playeragent.h"
#include <assistx2/tcphandler_wrapper.h>

class PlayerAgentImpl
{
public:
    PlayerAgentImpl();
    ~PlayerAgentImpl();
public:
    std::shared_ptr<assistx2::TcpHanlderWrapper> connector_;
};


PlayerAgent::PlayerAgent(std::shared_ptr<assistx2::TcpHanlderWrapper> connector):
 AgentBase(AgentType::PLAYER),
 pImpl_(new PlayerAgentImpl)
{
    pImpl_->connector_ = connector;
}

PlayerAgent::~PlayerAgent()
{
    DLOG(INFO) << "mid:=" << uid()
        << " Has been Destroyed!";
}

void PlayerAgent::SendTo(const assistx2::Stream& packet)
{
    assistx2::Stream clone(packet);
    clone.Insert(uid());

    pImpl_->connector_->SendTo(clone.GetNativeStream());
}

PlayerAgentImpl::PlayerAgentImpl()
{
}

PlayerAgentImpl::~PlayerAgentImpl()
{
}