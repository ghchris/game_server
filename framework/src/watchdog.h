/*=====================
��Ȩ(copyright statement):
����(author line): caocheng
======================*/
#ifndef FRAMEWORK_SRC_WATCHDOG_H_
#define FRAMEWORK_SRC_WATCHDOG_H_

#include "define.h"
#include <memory>

#include <boost/asio/io_service.hpp>

//WatchDog ���������û�����,�û���Ϣ����
//Ϊһ���µ����Ӵ���һ��agnet

class WatchDogImpl;
class WatchDog
{
public:
    explicit WatchDog(boost::asio::io_service & ios);
    virtual ~WatchDog();
    
    bool Initialize();

    //��������Ϸ�е�anget�Ƴ�������������ɸ�����������
    void RemoveAgent(uid_type uid);
private:
    DISALLOW_COPY_AND_ASSIGN(WatchDog);
private:
    std::unique_ptr< WatchDogImpl > pImpl_;
};

#endif //FRAMEWORK_SRC_WATCHDOG_H_
