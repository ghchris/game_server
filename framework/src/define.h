#ifndef FRAMEWORK_SRC_DEFINE_H_
#define FRAMEWORK_SRC_DEFINE_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

//�������ID����
#define uid_type std::uint32_t

#endif //FRAMEWORK_SRC_DEFINE_H_