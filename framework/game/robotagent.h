#ifndef FRAMEWORK_GAME_ROBOTRAGENT_H_
#define FRAMEWORK_GAME_ROBOTRAGENT_H_

#include "agentbase.h"

class RobotAgentImpl;
class RobotAgent :public AgentBase
{
public:
    RobotAgent();
    virtual ~RobotAgent();

    virtual bool Serialize(bool loadorsave);
    virtual void SendTo(const assistx2::Stream& packet);

    virtual MemberCommonGame* member_common_game();
    virtual MemberFides* member_fides();
    virtual MemberGame* member_game();
protected:
    void OnMessage(assistx2::Stream package);
private:
    std::unique_ptr< RobotAgentImpl > pImpl_;
};

#endif