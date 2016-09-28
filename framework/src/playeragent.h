#ifndef FRAMEWORK_SRC_PLAYERAGENT_H_
#define FRAMEWORK_SRC_PLAYERAGENT_H_

#include "agentbase.h"

namespace assistx2 {
    class TcpHanlderWrapper;
}

class PlayerAgentImpl;
class PlayerAgent:public AgentBase
{
public:
    explicit PlayerAgent(std::shared_ptr<assistx2::TcpHanlderWrapper> connector);
    virtual ~PlayerAgent();

    virtual void SendTo(const assistx2::Stream& packet);
private:
    std::unique_ptr< PlayerAgentImpl > pImpl_;
};

#endif