#pragma once
#ifndef __ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__UTILITY_HPP__
#define __ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__UTILITY_HPP__

#include <exception>
#include <functional>

#include "Windows.h"

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

#endif // !__ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__UTILITY_HPP__