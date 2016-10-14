#ifndef FRAMEWORK_SRC_DEFINE_H_
#define FRAMEWORK_SRC_DEFINE_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

//定义玩家ID类型
#define uid_type std::uint32_t

//服务器向xGateWay注册
const static std::int16_t CLINET_REQUEST_REGISTER = 10000;
const static std::int16_t SERVER_RESPONSE_REGISTER = 10000;

//standard. 标准路由包
const static std::int16_t STANDARD_ROUTE_PACKET = 10002;

static const std::int32_t	SESSION_TYPE_BEGIN = 0;
static const std::int32_t	SESSION_TYPE_AS_CLINET = 1;			//AS客户端连接
static const std::int32_t	SESSION_TYPE_GAME_SERVER = 2;			//游戏服务器	
static const std::int32_t	SESSION_TYPE_STORE = 3;			//商城
static const std::int32_t	SESSION_TYPE_LOGGER = 4;			//日志服务器
static const std::int32_t	SESSION_TYPE_ADMIN_TOOL = 5;			//管理工具
static const std::int32_t	SESSION_TYPE_END = 6;

#endif //FRAMEWORK_SRC_DEFINE_H_