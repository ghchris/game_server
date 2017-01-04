#include "csmajianglogic.h"
#include "cardgroup.h"

class CSMajiangLogicImpl
{
public:
    CSMajiangLogicImpl();
    ~CSMajiangLogicImpl();
public:

};

CSMajiangLogic::CSMajiangLogic():
 CardLogicBase(),
 pImpl_(new CSMajiangLogicImpl)
{

}

CSMajiangLogic::~CSMajiangLogic()
{

}

CSMajiangLogicImpl::CSMajiangLogicImpl()
{
}

CSMajiangLogicImpl::~CSMajiangLogicImpl()
{
}