#pragma once
#include<iostream>
#include<fstream>
#include<ctime>
#include<cstring>
#include<cmath>
#include<cstdlib>
#include<algorithm>
#include<memory>
#include<string>
#include<vector>
using namespace std;

#ifndef GDY
#define GDY
#define var auto
#ifdef _WIN64

#include<windows.h>
#include<wrl.h>
#include<tchar.h>

#ifdef UNICODE

#define Strcpy(a,b) wcscpy_s(a,b)
#define ERRORBLOCK(a) MessageBox(NULL, ,_T("Error"), ToLPCWSTR("Error"+a),0)

inline LPCWSTR ToLPCWSTR(string& orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t) *(origsize - 1));
	mbstowcs_s(nullptr, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

inline LPCWSTR ToLPCWSTR(char* l)
{
	string orig(l);
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t) *(orig.length() - 1));
	mbstowcs_s(nullptr, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

inline LPCWSTR ToLPCWSTR(const char* l)
{
	string orig(l);
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t) *(orig.length() - 1));
	mbstowcs_s(nullptr, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
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

inline HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size)
{
	using namespace Microsoft::WRL;

	CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
	extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
	extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
	extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
	extendedParams.lpSecurityAttributes = nullptr;
	extendedParams.hTemplateFile = nullptr;

	Wrappers::FileHandle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
	if (file.Get() == INVALID_HANDLE_VALUE)
	{
		throw std::exception();
	}

	FILE_STANDARD_INFO fileInfo = {};
	if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
	{
		throw std::exception();
	}

	if (fileInfo.EndOfFile.HighPart != 0)
	{
		throw std::exception();
	}

	*data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
	*size = fileInfo.EndOfFile.LowPart;

	if (!ReadFile(file.Get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
	{
		throw std::exception();
	}

	return S_OK;
}

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

template <typename T>
inline T* SafeAcquire(T* newObject)
{
	if (newObject != nullptr) ((IUnknown*)newObject)->AddRef();
	return newObject;
}

#else
#define Strcpy(a,b) wcscpy(a,b);
#endif // !UNICODE
#else
#ifndef UNICODE
#define Strcpy(a,b) strcpy_s(a,b);
#define stl(l) (l.c_str())

#endif
#endif // !_WIN64

template<typename T>
struct Range
{
	T maxn, minn;

	Range(T& _maxn, T& _minn)
	{
		if (typeid(T) != typeid(float) || typeid(T) != typeid(int) || typeid(T) != typeid(long)) 
			throw exception();
		maxn = _maxn;
		minn = _minn;
	}

	bool in(T& value)
	{
		if (value < minn) return false;
		if (value > maxn) return false;
		return true;
	}
};

inline void randomize()
{
	srand((unsigned)time(nullptr));
}

inline int random(int a)
{
	return rand() % a;
}

inline float random()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline int random(int a, int b)
{
	int c = rand() % b;
	while (c < a) c = rand() % b;
	return c;
}

inline float random(float a, float b)
{
	float scale = static_cast<float>(rand()) / RAND_MAX;
	float range = b - a;
	return scale * range + a;
}


#endif // !GDY