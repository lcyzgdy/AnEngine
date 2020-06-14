# 一个引擎系列

对于 Input、Timer 等全局统一对象，使用单例模式来管理，但将 AnEngine 转为动态库后，会出现 exe 中调用单例方法会获取一个和 dll 中不同的对象，导致单例对象实际上存在多个，使用各种方案后终于找到原因。

### 单例
在单例实现上，模仿以前看过的微软的 HoloToolkit-Unity 中的单例模式，使用了单例模板：
``` C++
template<typename T>
class Singleton : public NonCopyable
{
    friend typename T;
	template<typename U> friend class Singleton;

	static T* s_uniqueObj;

	virtual ~Singleton() = default;

public:
	__forceinline static T* Instance()
	{
		if (s_uniqueObj == nullptr)
		{
			s_uniqueObj = new T();
		}
		return s_uniqueObj;
	}
};
```
对于单例类，只需要继承该模板：
``` C++
class Input : public Singleton<Input>
{
    // Implemention
};

// 调用
Input::Instance()->Function();
```

### 问题
以上代码是在 DLL 中的，如果要在 EXE 中使用，则需要声明为导出。为了方便，定义相关宏：
``` C++
#ifdef _WINDLL
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
```
其中 _WINDLL 是 VS 预定义的输出类型为动态库的宏。因此单例模板声明如下：
``` C++
template<typename T>
class DLL_API Singleton : public NonCopyable
{
    // ...
}
```
为了方便测试，在 DLL 内实现测试函数：
``` C++
// Test.h
class Test1 : public Singleton<Test1>
{
public:
    void Log();
}

// Test.cpp
void Test1::Log()
{
    cout << this << endl;
}
```
在 EXE 中实现 main 函数：
``` C++
#include "Test.h"
using namespace std;
int main()
{
    Test1::Instance()->Log();
    cout << Test1::Instance() << endl;
    return 0;
}
```
编译运行，结果如下：
```
000001F02BBB55D0
000001D64AC00E90
```
输出单例对象的地址，得到了不一样的结果，显然这样实现单例是不正确的。通过参考网上资料，发现原因出现在模板上。

由于单例对象是写在头文件的，导致 Test.cpp（DLL） 和 main.cpp（EXE） 分别特化了一个 ```Singletion<Test1>::s_uniqueObj``` 对象，导致了调用 DLL 内函数和 EXE 的函数获得不同的对象。因此要避免在头文件中定义变量，对于头文件中定义的变量，应使用 extern 修饰，并在 CPP 文件中定义。因此，这里在 Test.cpp 中添加
``` C++
template<typename T>
T* Singleton<T>::s_uniqueObj = null;
```
但这样只能解决普通变量的问题，不能解决单例类的静态变量问题。

此时，该问题有另一种解决方案，不使用模板定义单例，将相关实现移动到每个需要使用的单例类的声明中。更多的，是定义一个宏：
``` C++
#define DECLEAR_SINGLETON(CLASS) class CLASS \
{\
    static CLASS* s_uniqueObj;\
public:\
	__forceinline static CLASS* Instance();\
	{\
		if (s_uniqueObj == nullptr)\
		{\
			s_uniqueObj = new CLASS();\
		}\
		return s_uniqueObj;\
	}\
```
``` C++
DECLEAR_SINGLETON(Test1)
public:
    void Log();
}
```

为了模仿 C#，这里还是使用模板实现。仔细分析就会发现，即使将定义放到 CPP 文件中，编译器在处理模板类的时候，会根据类型特化 Singleton，然而 DLL 和 EXE 里都需要头文件寻找声明，因此编译器在编译 DLL 和 EXE 时将模板特化了两次，也就导致 DLL 和 EXE 里实际上是两个不同的静态变量，分析编译后的代码也会发现，两个被特化的静态变量的签名还是有所不同，通过 typeid 得到 type_info 后判等也会得到 false。

因此解决方案也很容易，自己实现模板特化即可。
``` C++
// Singleton.h
template<typename T>
class Singleton : public NonCopyable
{
    friend typename T;
	template<typename U> friend class Singleton;

	static T* s_uniqueObj;

	virtual ~Singleton() = default;

public:
	__forceinline static T* Instance();
};

// Test.cpp
Test1::s_uniqueObj = nullptr;
template<>
Test1::Instance()
{
    // ...
}
```

再次运行 main.cpp，输出了相同的地址，问题解决。

### 参考资料
* <https://www.cnblogs.com/bourneli/archive/2011/12/28/2305280.html>
* <https://www.cnblogs.com/bqzhao/p/3531748.html>