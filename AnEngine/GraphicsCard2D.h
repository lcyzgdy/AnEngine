#pragma once
#ifndef __GRAPHICSCARD2D_H__
#define __GRAPHICSCARD2D_H__

#include "DX.h"
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <d3d11on12.h>
#include "RenderCoreConstants.h"
#include "Color.h"

namespace AnEngine::RenderCore
{
	extern procedure InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat);
}

namespace AnEngine::RenderCore::UI
{
	class GraphicsCard2D
	{
		friend procedure::AnEngine::RenderCore::InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat);

		Microsoft::WRL::ComPtr<ID2D1Factory3> m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device2> m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext2> m_d2dContext;
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		Microsoft::WRL::ComPtr<ID3D11On12Device> m_d3d11On12Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;

		Microsoft::WRL::ComPtr<ID3D11Resource> m_wrappedBackBuffers[r_DefaultFrameCount_const];
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_d2dRenderTarget[r_DefaultFrameCount_const];
		D2D1_BITMAP_PROPERTIES1 m_bitmapProperties;

		Microsoft::WRL::ComPtr<IDWriteFactory> m_dWriteFactory;

		procedure InitializeForText();

	public:
		GraphicsCard2D();
		~GraphicsCard2D() = default;

		void Initialize();
		void Release();

		ID3D11On12Device* GetDevice11On12();
		ID3D11Resource* GetWrappedBackBuffer(uint32_t index);

		void Begin();
		void End();

		ID2D1DeviceContext2* GetContext();

		void CreateTextFormat(IDWriteTextFormat** format);
		void CreateTextBrush(ID2D1SolidColorBrush** brush, const Resource::Color& color);
		void DrawText1(const wchar_t* text, uint32_t length, IDWriteTextFormat* format, ID2D1Brush* brush);
	};
}

#endif // !__GRAPHICSCARD2D_H__
