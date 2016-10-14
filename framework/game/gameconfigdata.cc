#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

#include "gameconfigdata.h"
#include "configmgr.h"


class GameConfigDataImpl
{
public:
    GameConfigDataImpl();
    ~GameConfigDataImpl();
    bool ParseRoomData();
public:
    std::vector< std::shared_ptr<RoomData> > rooms_data_;
};

GameConfigData::GameConfigData():
 pImpl_(new GameConfigDataImpl)
{

}

GameConfigData::~GameConfigData()
{

}

bool GameConfigData::Init()
{
    return pImpl_->ParseRoomData();
}

const std::vector< std::shared_ptr<RoomData> >& GameConfigData::rooms_data() const
{
    return pImpl_->rooms_data_;
}

GameConfigDataImpl::GameConfigDataImpl()
{
}

GameConfigDataImpl::~GameConfigDataImpl()
{
}

bool GameConfigDataImpl::ParseRoomData()
{
    auto game_obj = ConfigMgr::getInstance()->
        game_config_obj();

    auto rooms = game_obj->GetElement("systemroomcfg");
    if (rooms == nullptr)
    {
        throw std::invalid_argument("ASSERT(rooms == nullptr)");
    }

    for (auto item = rooms->FirstChildElement(); item != nullptr; item = item->NextSiblingElement())
    {
        auto roomcfg = std::make_shared<RoomData>();

        roomcfg->begin = atoi(item->Attribute("begin"));
        roomcfg->end = atoi(item->Attribute("end"));
        roomcfg->ju = atoi(item->Attribute("ju"));
        roomcfg->cost = atoi(item->Attribute("cost"));
        roomcfg->player = atoi(item->Attribute("player"));
        roomcfg->type = item->Attribute("type");
        roomcfg->name = item->Attribute("name");
        roomcfg->taxation = atoll(item->Attribute("taxation"));

        roomcfg->bettime = atoll(item->Attribute("bettime"));
        roomcfg->taxes_mode = atoi(item->Attribute("taxes_mode"));
        roomcfg->sb = atoi(item->Attribute("sb"));
        roomcfg->min = atol(item->Attribute("min"));
        roomcfg->max = atol(item->Attribute("max"));

        rooms_data_.push_back(roomcfg);
    }

    if (rooms_data_.empty())
    {
        throw std::invalid_argument("ASSERT(cfg.empty() == false)");
    }
    else
    {
        LOG(ERROR) << "roomscfg_:=" << rooms_data_.size();
    }

    return true;
}