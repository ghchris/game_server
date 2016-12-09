#ifndef FRAMEWORK_DB_LOGSERVER_H_
#define FRAMEWORK_DB_LOGSERVER_H_

#include <assistx2/singleton.h>
#include <memory>
#include <vector>
#include "define.h"

namespace assistx2 {
    class TcpHanlderWrapper;
}
class PrivateRoom;
class LogServerImpl;
class LogServer :public Singleton<LogServer>
{
public:
    LogServer();
    virtual ~LogServer();

    bool Initialize(std::shared_ptr<assistx2::TcpHanlderWrapper> connector);

    void WriteGoldLog(const uid_type mid, const std::int64_t gold_incr, 
        const  std::int64_t now_gold, const std::int32_t type, const  std::int32_t target = 0);
    void WriteSubGameLog(PrivateRoom* room,std::int32_t played_num, std::vector<std::int32_t> winner);
    void WriteGameLog(PrivateRoom* room, std::int32_t played_num);
private:
    std::unique_ptr< LogServerImpl > pImpl_;

};

#endif