#ifndef FRAMEWORK_SRC_COMMON_H_
#define FRAMEWORK_SRC_COMMON_H_

#include <json_spirit.h>
#include <memory>
#include "card.h"

class Agent;
class Common
{
public:
    Common() {};
    virtual ~Common() {};

    static json_spirit::Value StringToJson(const std::string& str, 
        json_spirit::Value_type type = json_spirit::obj_type);
    static bool PushServersStopped(std::shared_ptr< Agent > player);
    static void RemoveCard(std::shared_ptr<Card> card, Cards& cards);
};

#endif
