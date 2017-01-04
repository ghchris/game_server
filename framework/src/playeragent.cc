#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "playeragent.h"
#include "datalayer.h"
#include <assistx2/tcphandler_wrapper.h>
#include "logserver.h"
#include "gamedatamanager.h"

const static std::int16_t SERVER__UPDATE_GOLD = 1086;
const static std::int16_t SERVER_BROADCAST_GAME_ACCOUNT = 1031;//单局结算
const static std::int16_t SERVER_BROADCAST_ROOM_ACCOUNT = 1035;//服务器广播总结算
const static std::int16_t SERVER_BROADCAST_HAS_BEEN_DISBAND = 1013;
const static std::int16_t CLIENT_SET_GPS = 8001;

class PlayerAgentImpl
{
public:
    PlayerAgentImpl(PlayerAgent* owner);
    ~PlayerAgentImpl();
    void OnGoldChange();
public:
    PlayerAgent* owner_;
    std::shared_ptr<assistx2::TcpHanlderWrapper> connector_;
    MemberGame member_game_;
    MemberFides member_fides_;
    MemberCommonGame member_common_game_;
};


PlayerAgent::PlayerAgent(std::shared_ptr<assistx2::TcpHanlderWrapper> connector):
 AgentBase(AgentType::PLAYER),
 pImpl_(new PlayerAgentImpl(this))
{
    pImpl_->connector_ = connector;
}

PlayerAgent::~PlayerAgent()
{
 
}

bool PlayerAgent::Serialize(bool loadorsave)
{
    if (loadorsave == true)
    {
        if (DataLayer::getInstance()->membercommongame(uid(),
            pImpl_->member_common_game_) != 0)
        {
            DLOG(INFO) << "membercommongame failed mid:=" << uid();
            return false;
        }
        if (DataLayer::getInstance()->memberfides(uid(), pImpl_->member_fides_) != 0)
        {
            DLOG(INFO) << "memberfides failed mid:=" << uid();
            return false;
        }

        if (DataLayer::getInstance()->membergame(uid(), pImpl_->member_game_) != 0)
        {
            DLOG(INFO) << "membergame failed mid:=" << uid();
            return false;
        }
    }
    else
    {
        //now do nothing
    }

    return true;
}

void PlayerAgent::Process(assistx2::Stream * packet)
{
    const auto cmd = packet->GetCmd();
    switch (cmd)
    {
    case SERVER_BROADCAST_GAME_ACCOUNT:
    case SERVER_BROADCAST_ROOM_ACCOUNT:
    case SERVER_BROADCAST_HAS_BEEN_DISBAND:
        return GameDataManager::getInstance()->DeleteCmdStream(uid(),cmd);
    case CLIENT_SET_GPS:
    {
        auto str = packet->Read<std::string>();
        return set_gps(str);
    }
    default:
        break;
    }
    
    return AgentBase::Process(packet);
}

void PlayerAgent::SendTo(const assistx2::Stream& packet, bool needsave)
{
    assistx2::Stream clone(packet);
    clone.Insert(uid());

    if (needsave == true)
    {
        GameDataManager::getInstance()->SaveCmdStream(uid(),clone);
    }
    if (connect_status() == true)
    {
        pImpl_->connector_->SendTo(clone.GetNativeStream());
    }
}

//gold加金币为负，减金币为正
bool PlayerAgent::GoldPay(const std::int64_t gold,
    const std::int32_t pay_type)
{
    std::int64_t amount = 0;
    std::int64_t real_pay = 0;
    const int err = DataLayer::getInstance()->Pay(uid(), gold, amount, real_pay, false);
    if (err == 0)
    {
        LOG(INFO) << "PlayerAgent::GoldPay:" << ", mid:=" << uid() << ",gold_befor:" << 
            pImpl_->member_common_game_.gold() << ",delta:" << gold << ",amount:" << amount;

        pImpl_->member_common_game_.set_gold(amount);

        pImpl_->OnGoldChange();

        LogServer::getInstance()->WriteGoldLog(uid(), -gold, amount, pay_type);
        return true;
    }
    else
    {
        LOG(ERROR) << "PlayerAgent::GoldPay FALIED, err:=" << err << ", mid:=" << uid() << ", gold:=" << gold;

        return false;
    }
}

MemberCommonGame* PlayerAgent::member_common_game()
{
    return &pImpl_->member_common_game_;
}

MemberFides* PlayerAgent::member_fides()
{
    return &pImpl_->member_fides_;
}

MemberGame* PlayerAgent::member_game()
{
    return &pImpl_->member_game_;
}

PlayerAgentImpl::PlayerAgentImpl(PlayerAgent* owner):
 owner_(owner)
{
}

PlayerAgentImpl::~PlayerAgentImpl()
{
}

void PlayerAgentImpl::OnGoldChange()
{
    assistx2::Stream stream(SERVER__UPDATE_GOLD);
    stream.Write(member_common_game_.gold());
    stream.End();

    owner_->SendTo(stream);
}