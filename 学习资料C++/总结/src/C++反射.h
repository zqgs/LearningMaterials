#include <string>
#include <typeinfo>
#include <string.h>
#include <map>
using namespace std;

//外设消息处理抽象基类
class IPeripheral
{
public:
	IPeripheral(){}
	virtual ~IPeripheral(){}
	//外设消息处理
	virtual bool PeripheralMsgPro(const string& tsDeviceData){ return true; }//可以是任意函数，交由继承类实现。
};

// 动态对象抽象基类
class DynamicObject :public IPeripheral
{
public:
	DynamicObject() {}
	virtual ~DynamicObject() {}
};

// 动态对象创建工厂
class DynamicFactory
{
public:

	typedef DynamicObject* (*CreateFunction)();

	static DynamicFactory & Instance()
	{
		static DynamicFactory fac;
		return fac;
	}

	// 解析类型名称（转换为 A::B::C 的形式）
	static string ReadTypeName(const char * name)
	{

#ifndef __GNUC__
		const char * p = strstr(name, " ");
		if (p)
		{
			size_t prev_len = (size_t)(p - name);
			if (memcmp(name, "class", prev_len) == 0 ||
				memcmp(name, "struct", prev_len) == 0 ||
				memcmp(name, "enum", prev_len) == 0 ||
				memcmp(name, "union", prev_len) == 0)
			{
				p += 1;
				return string(p);
			}
		}

		return string(name);
#else
		tchar * real_name = abi::__cxa_demangle(name, nullptr, nullptr, nullptr);
		tstring real_name_string(real_name);
		free(real_name);
		return real_name_string;
#endif
	}

	bool Regist(const char * name, CreateFunction func)
	{
		if (!func)
		{
			return false;
		}
		string type_name = ReadTypeName(name);
		return _create_function_map.insert(std::make_pair(type_name, func)).second;
	}

	DynamicObject * Create(const string & type_name)
	{
		if (type_name.empty())
		{
			return NULL;
		}

		std::map<string, CreateFunction>::iterator it = _create_function_map.find(type_name);
		if (it == _create_function_map.end())
		{
			return NULL;
		}

		return it->second();
	}

	template<typename T>
	T * Create(const string & type_name)
	{
		DynamicObject * obj = Create(type_name);
		if (!obj)
		{
			return NULL;
		}
		T * real_obj = dynamic_cast<T*>(obj);
		if (!real_obj)
		{
			delete obj;
			return NULL;
		}
		return real_obj;
	}

public:

	std::map<string, CreateFunction> _create_function_map;
};

// 动态对象创建器
template<typename T>
class DynamicCreate : public DynamicObject
{
public:
	static DynamicObject * CreateObject()
	{
		return new T();
	}

	struct Registor
	{
		Registor()
		{
			tstring tsName = a2t(typeid(T).name());
			if (!DynamicFactory::Instance().Regist(tsName.c_str(), CreateObject))
			{
				assert(false);
			}
		}

		inline void do_nothing()const { }
	};

	static Registor s_registor;

public:
	DynamicCreate()
	{
		s_registor.do_nothing();
	}

	virtual ~DynamicCreate()
	{
		s_registor.do_nothing();
	}
};

template <typename T>
typename DynamicCreate<T>::Registor DynamicCreate<T>::s_registor;