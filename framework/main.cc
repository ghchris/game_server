#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <fstream>
#include <boost/asio.hpp>
#include <glog/logging.h>
#include <assistx2/platform_wrapper.h>
#include <gflags/gflags.h>
#include "configmgr.h"
#include "watchdog.h"
#include "datalayer.h"

DEFINE_string(cfg, "", "cfg file path.");

static bool ValidateCfg(const char * flagname, const std::string & value)
{
    std::fstream file(value.c_str(), std::ios::in);
    if (file.is_open() == false)
    {
        DLOG(ERROR) << "read config:=" << value << ", failed." << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}

void SignalHandle(const char* data, int size)
{
    std::string str = std::string(data, size);
    LOG(ERROR) << str;
}

int main(int argc, char ** argv)
{
    google::RegisterFlagValidator(&FLAGS_cfg, &ValidateCfg);

    google::ParseCommandLineFlags(&argc, &argv, true);

    google::InitGoogleLogging(argv[0]);

    google::InstallFailureSignalHandler();
    google::InstallFailureWriter(&SignalHandle);

    srand(static_cast<unsigned int>(time(nullptr)));

    boost::asio::io_service ios;
    boost::system::error_code ec;

    if ( !ConfigMgr::getInstance()->Init() )
    {
        LOG(INFO) << "LoadLocalCfg Failed";
        return 0;
    }

    char path[512] = { 0 };

    std::fstream pid;
    pid.open(ConfigMgr::getInstance()->pid_file_path(), std::fstream::trunc | std::fstream::out);
    pid << assistx2::os::GetCurrentDirectory(path, 511) << " " << assistx2::os::getpid();
    pid.close();

    auto watchdog = std::make_shared<WatchDog>(ios);
    if (watchdog == nullptr)
    {
        LOG(INFO) << "Create WatchDog Failed";
        return 0;
    }

    if ( !watchdog->Initialize() )
    {
        LOG(INFO) << "WatchDog Initialize Failed";
        return 0;
    }

    ios.run(ec);

    ConfigMgr::Destroy();

    google::ShutdownGoogleLogging();

    google::ShutDownCommandLineFlags();

    google::protobuf::ShutdownProtobufLibrary();
}