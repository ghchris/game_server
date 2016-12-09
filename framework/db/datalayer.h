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

    //���֧���ӿ�, incr �������ɸ� ���X������ ���X��ؓ
    std::int32_t Pay(const uid_type mid, const std::int64_t incr, std::int64_t &  amount, 
        std::int64_t& real_delta, bool bForce = false);

    void set_room_data_to_cache(const std::int32_t roomid, const std::string& data);
    bool room_data_from_cache(const std::int32_t roomid, std::string& data);

    void set_playing_player_to_cache(const uid_type mid, const std::int32_t session);
    void remove_playing_player_from_cache(const uid_type mid);

    bool proxy_mid(const uid_type mid,std::string& data);

    std::int32_t membercommongame(uid_type mid,MemberCommonGame& info,bool forcedflush = false);
    std::int32_t memberfides(uid_type mid, MemberFides& info, bool forcedflush = false);
    std::int32_t membergame(uid_type mid, MemberGame& info, bool forcedflush = false);
private:
    std::unique_ptr< DataLayerImpl > pImpl_;
};

#endif