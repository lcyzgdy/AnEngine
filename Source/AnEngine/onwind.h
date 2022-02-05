#pragma once
#ifndef __ONWIND_H__
#define __ONWIND_H__

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <functional>

#define var auto
#define 令(a) auto a
#define __FasterFunc(func) inline func __vectorcall

#ifdef _WINDOWS

#ifdef _WIN64

#ifdef _WINDLL
#define DLL_API __declspec(dllexport)
#else // _WINDLL
#define DLL_API __declspec(dllimport)
#endif // _WINDLL

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#endif /* __cplusplus */

#else	// _WIN64

#endif // !_WIN64

#else // _WINDOWS

#define DLL_API 

#endif // !_WINDOWS


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

template<typename T>
struct Range
{
	T m_maxn, m_minn;

	Range(const T& _minn, const T& _maxn) :m_maxn(_maxn), m_minn(_minn)
	{
		if (m_minn > m_maxn) throw std::exception("Min argument is greater than max argument");
	}

	bool Has(T& value)
	{
		if (value < m_minn) return false;
		if (value > m_maxn) return false;
		return true;
	}
	};

template<int Minn, int Maxn>
struct Range1
{
	constexpr bool operator()(int value)
	{
		if (value < Minn) return false;
		if (value > Maxn) return false;
		return true;
	}
};

template<int Maxn>
struct Range0
{
	constexpr bool operator()(int value)
	{
		if (value < 0) return false;
		if (value > Maxn) return false;
		return true;
	}
};

__forceinline void __vectorcall Randomize()
{
	srand((unsigned)time(nullptr));
}

__forceinline int __vectorcall Random(int a)
{
	return rand() % a;
}

__forceinline float __vectorcall Random()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

__forceinline int __vectorcall Random(int a, int b)
{
	int c = rand() % b;
	while (c < a) c = rand() % b;
	return c;
}

__forceinline float __vectorcall Random(float a, float b)
{
	float scale = static_cast<float>(rand()) / RAND_MAX;
	float range = b - a;
	return scale * range + a;
}


struct NonCopyable
{
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	~NonCopyable() = default;

	NonCopyable& operator=(const NonCopyable&) = delete;
};


#endif // !__ONWIND_H__