#pragma once
#ifndef __WIN32APP_H__
#define __WIN32APP_H__

#if defined(__WIN64) || defined(_WIN64) || defined(WIN64)

#include <Windows.h>
#include "onwind.h"

#include <d3d12.h>
#include "d3dx12.h"
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <dcomp.h>

#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_4.h>	// For WARP
#endif


namespace AnEngine
{
	// 描述Win32窗口
	// TODO：和GraphicContext解耦
	class DLL_API Win32App
	{
		HWND m_hwnd;
		uint32_t m_dxgiFactoryFlags;
		uint32_t m_windowWidth;
		uint32_t m_windowHeight;
		uint32_t m_swapBufferCount;
		bool m_dxgiDebugFlag;
		bool m_enableHdr;
		bool m_isStable;

		Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

		void Initialize();

	public:
		Win32App(HWND hwnd, uint32_t windowWidth, uint32_t windowHeighht, uint32_t swapBufferCount = 2, bool enableHdr = false,
			bool isStable = false);

		__forceinline void GetDxgiFactory(Microsoft::WRL::ComPtr<IDXGIFactory6>& ext)
		{
			m_dxgiFactory.CopyTo(ext.ReleaseAndGetAddressOf());
		}

		__forceinline void GetSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain4>& ext)
		{
			m_swapChain.CopyTo(ext.ReleaseAndGetAddressOf());
		}

		__forceinline uint32_t WindowWidth() { return m_windowWidth; }
		__forceinline uint32_t WindowHeight() { return m_windowHeight; }
		// TODO: 支持动态分辨率
	};
}
#endif // WIN64
#endif // !__WIN32APP_H__
