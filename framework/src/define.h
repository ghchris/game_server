#ifndef FRAMEWORK_SRC_DEFINE_H_
#define FRAMEWORK_SRC_DEFINE_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

//�������ID����
#define uid_type std::uint32_t

//��������xGateWayע��
const static std::int16_t CLINET_REQUEST_REGISTER = 10000;
const static std::int16_t SERVER_RESPONSE_REGISTER = 10000;

//standard. ��׼·�ɰ�
const static std::int16_t STANDARD_ROUTE_PACKET = 10002;

static const std::int32_t	SESSION_TYPE_BEGIN = 0;
static const std::int32_t	SESSION_TYPE_AS_CLINET = 1;			//AS�ͻ�������
static const std::int32_t	SESSION_TYPE_GAME_SERVER = 2;			//��Ϸ������	
static const std::int32_t	SESSION_TYPE_STORE = 3;			//�̳�
static const std::int32_t	SESSION_TYPE_LOGGER = 4;			//��־������
static const std::int32_t	SESSION_TYPE_ADMIN_TOOL = 5;			//������
static const std::int32_t	SESSION_TYPE_END = 6;

#endif //FRAMEWORK_SRC_DEFINE_H_