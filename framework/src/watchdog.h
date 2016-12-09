/*=====================
版权(copyright statement):
作者(author line): caocheng
======================*/
#ifndef FRAMEWORK_SRC_WATCHDOG_H_
#define FRAMEWORK_SRC_WATCHDOG_H_

#include "define.h"
#include <memory>

#include <boost/asio/io_service.hpp>

//WatchDog 用来管理用户连接,用户消息分配
//为一个新的连接创建一个agnet

class Agent;
class WatchDogImpl;
class WatchDog
{
public:
    explicit WatchDog(boost::asio::io_service & ios);
    virtual ~WatchDog();
    
    bool Initialize();

    //将不在游戏中的anget移除，具体操作交由各场景来管理
    void RemoveAgent(uid_type uid);
    std::shared_ptr<Agent> NewAgent(uid_type uid);
    std::shared_ptr<Agent> GetAgentByID(uid_type uid);
private:
    DISALLOW_COPY_AND_ASSIGN(WatchDog);
private:
    std::unique_ptr< WatchDogImpl > pImpl_;
};

#endif //FRAMEWORK_SRC_WATCHDOG_H_
