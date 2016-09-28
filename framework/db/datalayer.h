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

    std::int32_t membercommongame(uid_type mid,MemberCommonGame& info,bool forcedflush = false);
    std::int32_t memberfides(uid_type mid, MemberFides& info, bool forcedflush = false);
    std::int32_t membergame(uid_type mid, MemberGame& info, bool forcedflush = false);
private:
    std::unique_ptr< DataLayerImpl > pImpl_;
};

#endif