#include "common.h"
#include <json_spirit_reader_template.h>
#include <assistx2/tcphandler_wrapper.h>
#include "agent.h"

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

//服务器即将关闭
const static std::int16_t SERVER_PUSH_SERVERS_STOPPED = 1998;
extern bool g_server_stopped;

json_spirit::Value Common::StringToJson(const std::string& str,json_spirit::Value_type type)
{
    json_spirit::Value json;
    if (str.empty() == false && json_spirit::read_string(str, json) == true)
    {
        if (json.type() == type)
        {
            return json;
        }
    }
    return json;
}

bool Common::PushServersStopped(std::shared_ptr< Agent > player)
{
    if (g_server_stopped == true)
    {
        assistx2::Stream stream(SERVER_PUSH_SERVERS_STOPPED);
        stream.End();
        player->SendTo(stream);
        return true;
    }

    return false;
}

void Common::RemoveCard(std::shared_ptr<Card> card, Cards& cards)
{
    auto iter = std::find_if(cards.begin(), cards.end(), [card](const std::shared_ptr<Card> value) {
        return card->getName() == value->getName();
    });
    if (iter != cards.end())
    {
        cards.erase(iter);
    }
}