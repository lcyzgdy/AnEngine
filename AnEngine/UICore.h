#pragma once
#ifndef __UICORE_H__
#define __UICORE_H__

#include"DX.h"
#include<d2d1_3.h>
#include<dwrite_3.h>
#include<d3d11on12.h>
#include"RenderCoreConstants.h"

namespace AnEngine::RenderCore
{
	class UICore
	{
		Microsoft::WRL::ComPtr<ID2D1Factory3> m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device2> m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext2> m_d2dContext;
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		Microsoft::WRL::ComPtr<ID3D11On12Device> m_d3d11On12Device;

		Microsoft::WRL::ComPtr<IDWriteFactory> m_dWriteFactory;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_wrappedBackBuffers[r_DefaultFrameCount_const];
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_d2dRenderTarget[r_DefaultFrameCount_const];

	public:
		UICore();
		~UICore() = default;

		void Initialize();
		void Release();
	};
}

#endif // !__UICORE_H__
