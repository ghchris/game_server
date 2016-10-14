#ifndef FRAMEWORK_SRC_DATALAYER_H_
#define FRAMEWORK_SRC_DATALAYER_H_

#include <assistx2/singleton.h>
#include <memory>
#include "define.h"
#include "membergame.pb.h"
#include "memberfides.pb.h"
#include "membercommongame.pb.h"

class DataLayerImpl;
class DataLayer :public Singleton<DataLayer>
{
public:
    DataLayer(void);
    virtual ~DataLayer(void);

    bool Init();

    //金币支付接口, incr 可正，可负 扣錢為正， 加錢為負
    std::int32_t Pay(const uid_type mid, const std::int64_t incr, std::int64_t &  amount, 
        std::int64_t& real_delta, bool bForce = false);

    std::int32_t membercommongame(uid_type mid,MemberCommonGame& info,bool forcedflush = false);
    std::int32_t memberfides(uid_type mid, MemberFides& info, bool forcedflush = false);
    std::int32_t membergame(uid_type mid, MemberGame& info, bool forcedflush = false);
private:
    std::unique_ptr< DataLayerImpl > pImpl_;
};

#endif