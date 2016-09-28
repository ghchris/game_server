#ifndef FRAMEWORK_SRC_WATCHDOG_H_
#define FRAMEWORK_SRC_WATCHDOG_H_

#include "define.h"
#include <memory>

#include <boost/asio/io_service.hpp>

class WatchDogImpl;
class WatchDog
{
public:
    explicit WatchDog(boost::asio::io_service & ios);
    virtual ~WatchDog();

    bool Initialize();
    void RemoveAgent(uid_type uid);
private:
    DISALLOW_COPY_AND_ASSIGN(WatchDog);
private:
    std::unique_ptr< WatchDogImpl > pImpl_;
};

#endif //FRAMEWORK_SRC_WATCHDOG_H_
