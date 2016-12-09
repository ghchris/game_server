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

    virtual bool Serialize(bool loadorsave);

    virtual void Process(assistx2::Stream * packet);

    virtual void SendTo(const assistx2::Stream& packet, bool needsave = false);

    virtual bool GoldPay(const std::int64_t gold,
        const std::int32_t pay_type);

    virtual MemberCommonGame* member_common_game();
    virtual MemberFides* member_fides() ;
    virtual MemberGame* member_game();
private:
    std::unique_ptr< PlayerAgentImpl > pImpl_;
};

#endif