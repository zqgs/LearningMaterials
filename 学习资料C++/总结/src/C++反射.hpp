#include "C++反射.h"

//动态对象1
class INIT : public DynamicCreate<INIT>
{
public:
	bool PeripheralMsgPro(const string& tsDeviceData) override
    {
        //具体实现!!!
    }
};
//动态对象2
class InitMessageDesign :
	public DynamicCreate<InitMessageDesign>
{
public:
	bool PeripheralMsgPro(const string& tsDeviceData) override
    {
        //具体实现!!!
    }
};
//动态对象n依次类推



//调用
void CallObj()
{
    IPeripheral * p1 = DynamicFactory::Instance().Create<IPeripheral>("INIT");
	if (p1)
		p1->PeripheralMsgPro("hello");
     p1 = DynamicFactory::Instance().Create<IPeripheral>("InitMessageDesign");
	if (p1)
		p1->PeripheralMsgPro("world");
}