#pragma once

#ifndef __ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__DIRECTX12FACTORY_H__
#define __ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__DIRECTX12FACTORY_H__

#include <wrl.h>

#include "onwind.h"
#include "Singleton.h"
#include "DirectX12.h"

namespace AnEngine::PlatformDependence::Rendering
{
	/// <summary>
	/// 工厂类，用来生产各种D3D12对象
	/// </summary>
	class DirectX12Factory : public AnEngine::Utility::Singleton<DirectX12Factory>
	{
		friend class Utility::Singleton<DirectX12Factory>;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

	private:
		void InitializeSwapChain(int width, int height, int swapBufferCount, HWND hwnd, DXGI_FORMAT dxgiFormat);

	public:
		void Initialize();



		inline ID3D12Device* GetDevice() { return m_device.Get(); }
	};
}

#endif // !__ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__DIRECTX12FACTORY_H__
