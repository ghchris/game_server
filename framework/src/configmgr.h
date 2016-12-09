#ifndef FRAMEWORK_SRC_CONFIGMGR_H_
#define FRAMEWORK_SRC_CONFIGMGR_H_

#include <assistx2/database_wrapper.h>
#include <assistx2/singleton.h>
#include <memory>
#include <string>

class Configure;
class ConfigMgrImpl;
class ConfigMgr : public Singleton<ConfigMgr >
{
private:
    friend class DefaultBuilder<ConfigMgr >;

    ConfigMgr(void);
    virtual ~ConfigMgr(void);
public:
    bool Init();
    const std::shared_ptr<Configure> app_config_obj() const;
    const std::shared_ptr<Configure> game_config_obj() const;
    const std::uint32_t server_id() const;
    const std::string& pid_file_path() const;
    const DBConfig_type& db_config() const;
    const std::string server_version() const;
private:
    std::unique_ptr< ConfigMgrImpl > pImpl_;
};

#endif //FRAMEWORK_SRC_CONFIGMGR_H_

