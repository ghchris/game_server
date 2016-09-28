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
public:
    std::shared_ptr< Database > database_client_;
    std::shared_ptr< IMemcacheHandler > memcached_client_;
    std::string prefix_;
};

DataLayer::DataLayer(void)
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

std::int32_t DataLayer::membercommongame(uid_type mid, MemberCommonGame& info, 
    bool forcedflush )
{
    std::string sql_buffer;

    try
    {
        sql_buffer = (boost::format("SELECT  * FROM `%1%`.`membercommongame0`  WHERE `mid` = %2%;") %
            pImpl_->prefix_ % mid).str();

        DLOG(INFO) << "SQL:=" << sql_buffer;
    }
    catch (std::exception & e)
    {
        LOG(INFO) << "GetMemberGameFromDB, FAILED, exception:=" << e.what();
        return -1;
    }

    std::unique_ptr<IQueryResult> result(pImpl_->database_client_->PQuery(sql_buffer));
    if (result.get() == nullptr || result->RowCount() == 0)
    {
        LOG(ERROR) << "GetMemberGameFromDB, FAILED mid:=" << mid;
        return -1;
    }

    json_spirit::Value array;
    pImpl_->DataBaseToJson(result.get(), MemberCommonGame::descriptor(), array);

    return 0;
}

std::int32_t DataLayer::memberfides(uid_type mid, MemberFides& info, 
    bool forcedflush )
{
    return 0;
}

std::int32_t DataLayer::membergame(uid_type mid, MemberGame& info, 
    bool forcedflush)
{
    return 0;
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
    DCHECK_NOTNULL(reflection);

    for (int i = 0; i < descriptor->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor * field_descriptor = descriptor->field(i);
        DCHECK_NOTNULL(field_descriptor);

        // ÉèÖÃÖµ
        switch (field_descriptor->type())
        {
        case ::google::protobuf::FieldDescriptor::TYPE_STRING:
        {
            reflection->SetString(msg, field_descriptor, assistx2::ToString(array.at(i)));
            DLOG(INFO) << "name:=" << field_descriptor->name() << ", str:=" << assistx2::ToString(array.at(i)) << ", index:=" << i
                << ", " << json_spirit::write_string(json);
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
    DCHECK_NOTNULL(reflection);

    auto descriptor = msg->GetDescriptor();

    json_spirit::Array array;

    for (int i = 0; i < descriptor->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor * field_descriptor = descriptor->field(i);
        DCHECK_NOTNULL(field_descriptor);

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
        DCHECK_NOTNULL(field_descriptor);

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
