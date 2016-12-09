#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <glog/logging.h>
#include <assistx2/database.h>
#include <assistx2/connector.h>
#include <assistx2/configure.h>
#include <assistx2/memcached_wrapper.h>
#include <thread>
#include <json_spirit_writer_template.h>
#include <json_spirit_reader_template.h>
#include <assistx2/json_wrapper.h>
#include <google/protobuf/message.h>
#include <stdexcept>
#include <boost/format.hpp>

#include "datalayer.h"
#include "configmgr.h"

class DataLayerImpl
{
public:
    DataLayerImpl();
    ~DataLayerImpl();
    std::int32_t JsonToMessage(json_spirit::Value & json, ::google::protobuf::Message * msg);
    void MessageToJson(::google::protobuf::Message * msg, json_spirit::Value & json);
    void DataBaseToJson(IQueryResult * result, const ::google::protobuf::Descriptor * descriptor, json_spirit::Value & json);
    std::unique_ptr<IQueryResult> QueryUserInfo(uid_type mid,const std::string& sql);
public:
    std::shared_ptr< Database > database_client_;
    std::shared_ptr< IMemcacheHandler > memcached_client_;
    std::string prefix_;
};

DataLayer::DataLayer(void):
 pImpl_(new DataLayerImpl)
{

}

DataLayer::~DataLayer(void)
{

}

bool DataLayer::Init()
{
    std::string libmemcached_cfg;
    auto reader = ConfigMgr::getInstance()->app_config_obj();
    CHECK(reader->getConfig("libmemcached", "memcached", libmemcached_cfg));

    LOG(INFO) << "InitMemcachedAgent memcached:=" << libmemcached_cfg;

    pImpl_->memcached_client_ = IMemcacheHandler::CreateMemcacheHandler(libmemcached_cfg);
    CHECK_NOTNULL(pImpl_->memcached_client_.get());

    std::stringstream key;
    key << "test" << std::this_thread::get_id();

    std::string  value;

    CHECK(pImpl_->memcached_client_->set(key.str().c_str(), "memcached test OK!"));
    CHECK(pImpl_->memcached_client_->get(key.str().c_str(), value));
    LOG(INFO) << "InitMemcachedAgent " << value;
    CHECK(pImpl_->memcached_client_->remove(key.str().c_str()));

    CHECK(reader->getConfig("system", "prefix", pImpl_->prefix_) == true);

    pImpl_->database_client_ = std::make_shared<Database>();
    CHECK_NOTNULL(pImpl_->database_client_.get());

    auto dbcfg = ConfigMgr::getInstance()->db_config();
    if (pImpl_->database_client_->Connect(dbcfg) == false)
    {
        LOG(ERROR) << "database_client_ Connect: Failed";
        return false;
    }

    return true;
}

std::int32_t DataLayer::Pay(const uid_type mid, const std::int64_t incr, std::int64_t &  amount,
    std::int64_t& real_delta, bool bForce)
{
    std::stringstream key;
    key << "TMGMCOM" << mid;

    pImpl_->memcached_client_->remove(key.str());

    std::unique_ptr<IQueryResult> result(pImpl_->database_client_->PQuery("CALL `%s`.`pay`(%d, %d, %s, @err, @old_gold, @new_gold); "
        " SELECT @err, @old_gold, @new_gold;", pImpl_->prefix_.c_str(),mid, -incr, (bForce ? "TRUE" : "FALSE")));
    if (result.get() == nullptr || result->RowCount() == 0)
    {
        LOG(ERROR) << "Pay, FAILED mid:=" << mid << ", " << pImpl_->database_client_->GetLastError();
        return -1;
    }

    auto err = result->GetItemLong(0, "@err");
    amount = result->GetItemLong(0, "@new_gold");
    real_delta = result->GetItemLong(0, "@old_gold") - amount;

    DLOG(INFO) << "Pay mid:=" << mid << ", incr:=" << incr << ", new_gold:=" << amount << ", real_delta:=" << real_delta
        << ", bForce:=" << (bForce ? "TRUE" : "FALSE") << ", err:=" << err
        << ", old_gold:=" << result->GetItemLong(0, "@old_gold");

    return err;
}

void DataLayer::set_room_data_to_cache(const std::int32_t roomid, const std::string& data)
{
    std::stringstream key;
    key << "MAJIANGROOM|" << roomid;

    if (data.empty() == false)
    {
        DLOG(INFO) << "set_room_data_to_cache:" << key.str() << ", " << data;

        std::string value;
        if (pImpl_->memcached_client_->get(key.str(), value) == true)
        {
            if (pImpl_->memcached_client_->replace(key.str(), data) == false)
            {
                LOG(INFO) << ("DataLayer::set_room_data_to_cache. FAILED.");
            }
        }
        else
        {
            if (pImpl_->memcached_client_->set(key.str(), data) == false)
            {
                LOG(INFO) << ("DataLayer::set_room_data_to_cache. FAILED.");
            }
        }
    }
    else
    {
        DLOG(INFO) << "remove_room_data_to_cache:" << key.str() ;

        if (pImpl_->memcached_client_->remove(key.str()) == false)
        {
            LOG(INFO) << ("DataLayer::remove_room_data_to_cache. FAILED.");
        }
    }
}

bool DataLayer::room_data_from_cache(const std::int32_t roomid, std::string& data)
{
    std::stringstream key;
    key << "MAJIANGROOM|" << roomid;

    if (pImpl_->memcached_client_->get(key.str(), data) == true)
    {
        if (data.empty())
        {
            pImpl_->memcached_client_->remove(key.str());
            return false;
        }
        else
        {
            DLOG(INFO) << "room_data_from_cache:" << key.str() << ", " << data;
            return true;
        }
    }
    else
    {
        return false;
    }
}

void DataLayer::set_playing_player_to_cache(const uid_type mid, const std::int32_t session)
{
    std::stringstream key;
    key << "PLAYERSTATUS|" << mid;

    std::stringstream val;
    val << session;

    DLOG(INFO) << "set_playing_player_to_cache:" << key.str() << ", " << session;

    std::string value;
    if (pImpl_->memcached_client_->get(key.str(), value) == true)
    {
        if (pImpl_->memcached_client_->replace(key.str(), val.str()) == false)
        {
            LOG(INFO) << ("DataLayer::set_playing_player_to_cache. FAILED.");
        }
    }
    else
    {
        if (pImpl_->memcached_client_->set(key.str(), val.str()) == false)
        {
            LOG(INFO) << ("DataLayer::set_playing_player_to_cache. FAILED.");
        }
    }
}

void DataLayer::remove_playing_player_from_cache(const uid_type mid)
{
    std::stringstream key;
    key << "PLAYERSTATUS|" << mid;

    DLOG(INFO) << "remove_playing_player_from_cache:" << key.str();

    if (pImpl_->memcached_client_->remove(key.str()) == false)
    {
        LOG(INFO) << ("DataLayer::remove_playing_player_from_cache. FAILED.");
    }
}

bool DataLayer::proxy_mid(const uid_type mid, std::string& data)
{
    std::stringstream key;
    key << "ROPEN|" << mid;

    if (pImpl_->memcached_client_->get(key.str(), data) == true)
    {
        if (data.empty())
        {
            pImpl_->memcached_client_->remove(key.str());
            return false;
        }
        else
        {
            DLOG(INFO) << "proxy_mid:" << key.str() << ", " << data;
            return true;
        }
    }
    else
    {
        return false;
    }
}

std::int32_t DataLayer::membercommongame(uid_type mid, MemberCommonGame& info, 
    bool forcedflush )
{
    std::stringstream key;
    key << "TMGMCOM" << mid;

    if (forcedflush == false)
    {
        std::string value;
        if (pImpl_->memcached_client_->get(key.str(), value) == true && value.empty() == false)
        {
            DLOG(INFO) << "membercommongame mid:=" << mid << ", value:=" << value;
            json_spirit::Value json;
            if (json_spirit::read_string(value, json) == true)
            {
                pImpl_->JsonToMessage(json, &info);
                return 0;
            }
        }
    }

    auto result = pImpl_->QueryUserInfo(mid, 
        "SELECT  * FROM `%1%`.`membercommongame0`  WHERE `mid` = %2% ;");
    if (result == nullptr)
    {
        return -1;
    }
    
    json_spirit::Value array;
    pImpl_->DataBaseToJson(result.get(), MemberCommonGame::descriptor(), array);

    auto res = pImpl_->JsonToMessage(array, &info);
    if (res == 0)
    {
        const std::string json_str = json_spirit::write_string(array);
        DLOG(INFO) << "SetCommonGameInfoToCache:=" << json_str << ", mid:=" << mid;
        if (pImpl_->memcached_client_->set(key.str(), json_str, 0) == true)
        {
            return 0;
        }
        else
        {
            LOG(INFO) << "SetCommonGameInfoToCache. WRITE TO CACHED FAILED mid:=" << mid;
            return -1;
        }
    }

    return res;
}

std::int32_t DataLayer::memberfides(uid_type mid, MemberFides& info, 
    bool forcedflush )
{
    std::stringstream key;
    key << "TMFIELD" << mid;

    if (forcedflush == false)
    {
        std::string value;
        if (pImpl_->memcached_client_->get(key.str(), value) == true && value.empty() == false)
        {
            //DLOG(INFO) << "mid:=" << mid << ", " << value;

            json_spirit::Value json;
            if (json_spirit::read_string(value, json) == true)
            {
                pImpl_->JsonToMessage(json, &info);
                return 0;
            }
        }
    }
    
    auto result = pImpl_->QueryUserInfo(mid,
        "SELECT * FROM `%1%`.`memberfides0`  WHERE `mid` = %2%;");
    if (result == nullptr)
    {
        return -1;
    }

    json_spirit::Value array;
    pImpl_->DataBaseToJson(result.get(), MemberFides::descriptor(), array);

    auto res = pImpl_->JsonToMessage(array, &info);
    if (res == 0)
    {
        auto json_str = json_spirit::write_string(array, json_spirit::raw_utf8);

        DLOG(INFO) << "mid:=" << mid << ", " << json_str;
        pImpl_->memcached_client_->set(key.str(), json_str, 0);
    }

    return 0;
}

std::int32_t DataLayer::membergame(uid_type mid, MemberGame& info, 
    bool forcedflush)
{
    std::stringstream key;
    key << "TMGM" << mid << "|3";

    if (forcedflush == false)
    {
        std::string value;
        if (pImpl_->memcached_client_->get(key.str(), value) == true && value.empty() == false)
        {
            DLOG(INFO) << "membergame mid:=" << mid << ", value:=" << value;
            json_spirit::Value json;
            if (json_spirit::read_string(value, json) == true)
            {
                pImpl_->JsonToMessage(json, &info);
                return 0;
            }
        }
    }

    auto result = pImpl_->QueryUserInfo(mid,
        "SELECT  * FROM `%1%`.`membergame0` WHERE `mid` = %2% AND `type` = 3;");
    if (result == nullptr)
    {
        return -1;
    }

    json_spirit::Value array;
    pImpl_->DataBaseToJson(result.get(), MemberGame::descriptor(), array);

    auto res = pImpl_->JsonToMessage(array, &info);
    if (res == 0)
    {
        const std::string json_str = json_spirit::write_string(array);

        if (pImpl_->memcached_client_->set(key.str(), json_str, 0) == true)
        {
            return 0;
        }
        else
        {
            LOG(INFO) << "GetPlayerGameInfo. WRITE TO CACHED FAILED mid:=" << mid;
            return -1;
        }
    }

    return res;
}

DataLayerImpl::DataLayerImpl()
{
}

DataLayerImpl::~DataLayerImpl()
{
}

std::int32_t DataLayerImpl::JsonToMessage(json_spirit::Value & json, ::google::protobuf::Message * msg)
{
    if (json.type() != json_spirit::array_type)
    {
        LOG(INFO) << "Failed Message name:=" << msg->GetDescriptor()->name()
            << ", json:=" << json_spirit::write_string(json);

        return -1;
    }

    auto descriptor = msg->GetDescriptor();

    auto & array = json.get_array();
    //DCHECK_EQ(array.size(), static_cast<size_t>(descriptor->field_count()) );
    if (array.size() != static_cast<size_t>(descriptor->field_count()))
    {
        LOG(INFO) << "Failed Message name:=" << msg->GetDescriptor()->name()
            << ", json:=" << json_spirit::write_string(json);

        return -2;
    }

    const google::protobuf::Reflection * reflection = msg->GetReflection();
    DCHECK(reflection != nullptr);

    for (int i = 0; i < descriptor->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor * field_descriptor = descriptor->field(i);
        DCHECK(field_descriptor != nullptr);

        // ÉèÖÃÖµ
        switch (field_descriptor->type())
        {
        case ::google::protobuf::FieldDescriptor::TYPE_STRING:
        {
            reflection->SetString(msg, field_descriptor, assistx2::ToString(array.at(i)));
            //DLOG(INFO) << "name:=" << field_descriptor->name() << ", str:=" << assistx2::ToString(array.at(i)) << ", index:=" << i
             //   << ", " << json_spirit::write_string(json);
        }
        break;
        case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
        case ::google::protobuf::FieldDescriptor::TYPE_INT32:
            reflection->SetInt32(msg, field_descriptor, assistx2::ToInt(array.at(i)));
            break;
        case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
        case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            reflection->SetInt64(msg, field_descriptor, assistx2::ToInt64(array.at(i)));
            break;
        default:
            DCHECK(false) << "UNKNOWN type:=" << field_descriptor->type();
            break;
        }
    }

    return 0;
}

void DataLayerImpl::MessageToJson(::google::protobuf::Message * msg, json_spirit::Value & json)
{
    const google::protobuf::Reflection * reflection = msg->GetReflection();
    DCHECK(reflection != nullptr);

    auto descriptor = msg->GetDescriptor();

    json_spirit::Array array;

    for (int i = 0; i < descriptor->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor * field_descriptor = descriptor->field(i);
        DCHECK(field_descriptor != nullptr);

        switch (field_descriptor->type())
        {
        case ::google::protobuf::FieldDescriptor::TYPE_STRING:
        {
            array.push_back(reflection->GetString(*msg, field_descriptor));
        }
        break;
        case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
        case ::google::protobuf::FieldDescriptor::TYPE_INT32:
        {
            array.push_back(assistx2::ToInt(array.at(i)));
        }
        break;
        case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
        case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            array.push_back(assistx2::ToInt64(array.at(i)));
            break;
        default:
            DCHECK(false) << "UNKNOWN type:=" << field_descriptor->type();
            break;
        }
    }
}

void DataLayerImpl::DataBaseToJson(IQueryResult * result, const ::google::protobuf::Descriptor * descriptor, json_spirit::Value & json)
{
    json_spirit::Array array;

    for (int i = 0; i < descriptor->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor * field_descriptor = descriptor->field(i);
        DCHECK(field_descriptor != nullptr);
        
        switch (field_descriptor->type())
        {
        case ::google::protobuf::FieldDescriptor::TYPE_STRING:
        {
            array.push_back(result->GetItemString(0, field_descriptor->name()));
        }
        break;
        case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
        case ::google::protobuf::FieldDescriptor::TYPE_INT32:
        case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
        case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            array.push_back(result->GetItemLong(0, field_descriptor->name()));
            break;
        default:
            DCHECK(false) << "UNKNOWN type:=" << field_descriptor->type();
            break;
        }
    }

    json = array;
}

std::unique_ptr<IQueryResult> DataLayerImpl::QueryUserInfo(uid_type mid,const std::string& sql)
{
    std::string sql_buffer;
    try
    {
        sql_buffer = (boost::format(sql.c_str()) %
            prefix_ % mid).str();

        DLOG(INFO) << "SQL:=" << sql_buffer;
    }
    catch (std::exception & e)
    {
        LOG(INFO) << "QueryUserInfo, FAILED, exception:=" << e.what();
        return nullptr;
    }

    std::unique_ptr<IQueryResult> result(database_client_->PQuery(sql_buffer));
    if (result.get() == nullptr || result->RowCount() == 0)
    {
        LOG(ERROR) << "QueryUserInfo, FAILED mid:=" << mid;
        return nullptr;
    }

    return std::move(result);
}