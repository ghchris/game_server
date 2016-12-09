#ifndef _FRAMEWORK_TEST_TESTCARDMANAGER_H_
#define _FRAMEWORK_TEST_TESTCARDMANAGER_H_

#include <assistx2/singleton.h>
#include <memory>
#include "card.h"

class TestCardManagerImpl;
class TestCardManager :public Singleton<TestCardManager>
{
public:
    TestCardManager();
    virtual ~TestCardManager();
    void Update();
    void GetCustomCard(Cards& cards);
private:
    std::unique_ptr< TestCardManagerImpl > pImpl_;
};

#endif // 
