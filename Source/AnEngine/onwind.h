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

#include <Windows.h>

#ifdef _WINDLL
#define DLL_API __declspec(dllexport)
#else // _WINDLL
#define DLL_API __declspec(dllimport)
#endif // _WINDLL



#ifdef UNICODE

constexpr auto SOLUTION_DIR = L"C:\\Users\\PC\\Documents\\Code\\VSProject\\AnEngine";

#define Strcpy(a,b) wcscpy_s(a, b)
#define ERRORBLOCK(a) MessageBox(NULL, ToLPCWSTR(a), L"Error", 0)

#if defined _DEBUG || defined DEBUG
#define ERRORBREAK(a) {\
						ERRORBLOCK(a); \
						throw std::exception(); \
					  }
#else
#define ERRORBREAK(a) (a)
#endif // _DEBUG || DEBUG

inline LPCWSTR ToLPCWSTR(std::string& orig)
{
	size_t origsize = orig.length() + 1;
	wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (origsize - 1));
	mbstowcs_s(nullptr, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}


inline LPCWSTR ToLPCWSTR(const char* l)
{
	size_t origsize = strlen(l) + 1;
	wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * origsize);
	mbstowcs_s(nullptr, wcstring, origsize, l, _TRUNCATE);
	return wcstring;
}

inline std::string WStringToString(const std::wstring& wl)
{

}

inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
	if (path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception("一个奇怪的错误");
	}
}

inline void ThrowIfFailed(HRESULT hr, const std::function<void(void)>& f)
{
	if (FAILED(hr))
	{
		f();
		throw std::exception("一个奇怪的错误");
	}
}
inline void ThrowIfFalse(bool value)
{
	ThrowIfFailed(value ? S_OK : E_FAIL);
}

inline void ThrowIfFalse(bool value, const wchar_t* msg)
{
	ThrowIfFailed(value ? S_OK : E_FAIL);
}

#else	// UNICODE
#define Strcpy(a,b) wcscpy(a,b);
#define SOLUTION_DIR "C:\\Users\\PC\\Documents\\Code\\VSProject\\AnEngine"
#endif // !UNICODE
#else	// _WIN64
#ifndef UNICODE
#define Strcpy(a,b) strcpy_s(a,b);
#define stl(l) (l.c_str())

#endif
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