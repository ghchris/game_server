#define GLOG_NO_ABBREVIATED_SEVERITIES

#include "configmgr.h"
#include <assistx2/configure.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <tinyxml.h>

DECLARE_string(cfg);

class ConfigMgrImpl
{
public:
    ConfigMgrImpl();
    ~ConfigMgrImpl();

    void LoadAppConfig();
public:
    std::shared_ptr<Configure> app_config_obj_;
    std::shared_ptr<Configure> game_config_obj_;
    std::int32_t server_id_ = 0;
    std::string pid_file_path_;
    DBConfig_type db_config_;
    std::string version_;
};

ConfigMgr::ConfigMgr(void):
 pImpl_(new ConfigMgrImpl)
{

}

ConfigMgr::~ConfigMgr(void)
{

}

bool ConfigMgr::Init()
{
    try
    {
        pImpl_->LoadAppConfig();
    }
    catch (std::exception & e)
    {
        DLOG(INFO) << e.what();
        return false;
    }

    return true;
}

const std::shared_ptr<Configure> ConfigMgr::app_config_obj() const
{
    return pImpl_->app_config_obj_;
}

const std::shared_ptr<Configure> ConfigMgr::game_config_obj() const
{
    return pImpl_->game_config_obj_;
}

const std::uint32_t ConfigMgr::server_id() const
{
    return pImpl_->server_id_;
}

const std::string& ConfigMgr::pid_file_path() const
{
    return pImpl_->pid_file_path_;
}

const DBConfig_type& ConfigMgr::db_config() const
{
    return pImpl_->db_config_;
}

const std::string ConfigMgr::server_version() const
{
    return pImpl_->version_;
}

ConfigMgrImpl::ConfigMgrImpl()
{
}

ConfigMgrImpl::~ConfigMgrImpl()
{
}

void ConfigMgrImpl::LoadAppConfig()
{
    app_config_obj_ = std::make_shared<Configure>();
    if (app_config_obj_->Load(FLAGS_cfg) != 0)
    {
        LOG(ERROR) << ("ConfigMgr::update. FAILED READ config.xml");
        throw std::runtime_error("ConfigMgr::LoadAppCfg, assert(app_cfg_->Load(FLAGS_cfg) == false) ");
    }

    std::string game_cfg_path;
    app_config_obj_->getConfig("cfg", "path", game_cfg_path);

    game_config_obj_ = std::make_shared<Configure>();
    if (game_config_obj_->Load(game_cfg_path) != 0)
    {
        LOG(ERROR) << "ASSERT(game_cfg_path == nullptr). game_cfg_path:=" << game_cfg_path;
        throw std::runtime_error("assert (game_cfg_->Load(game_cfg_path) != 0)");
    }

    app_config_obj_->getConfig("APP", "SID", server_id_);

    app_config_obj_->getConfig("APP", "PID", pid_file_path_);

    app_config_obj_->getConfig("APP", "VERSION", version_);

    TiXmlElement * pGameItems = app_config_obj_->GetElement("DB");
    if (pGameItems != nullptr)
    {
        db_config_.host = pGameItems->Attribute("host");
        db_config_.port = pGameItems->Attribute("port");
        db_config_.passwd = pGameItems->Attribute("passwd");
        db_config_.db = pGameItems->Attribute("db");
        db_config_.user = pGameItems->Attribute("user");
    }

    LOG(INFO) << "HOST:=" << db_config_.host << ", PORT:=" << db_config_.port << ", USER:=" << db_config_.user
        << ", PW:=" << db_config_.passwd << ", DB:=" << db_config_.db;
}