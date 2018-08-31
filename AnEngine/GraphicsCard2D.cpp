#include "GraphicsCard2D.h"
#include "RenderCore.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")

using namespace Microsoft::WRL;

namespace AnEngine::RenderCore::UI
{
	procedure GraphicsCard2D::InitializeForText()
	{
		ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory));
	}

	GraphicsCard2D::GraphicsCard2D()
	{

	}

	void GraphicsCard2D::Initialize()
	{
		uint32_t dxgiFactoryFlags = 0;
		uint32_t d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

		ComPtr<ID3D11Device> d3d11Device;
		ThrowIfFailed(D3D11On12CreateDevice(r_graphicsCard[0]->GetDevice(), d3d11DeviceFlags, nullptr, 0,
			reinterpret_cast<IUnknown**>(r_graphicsCard[0]->GetCommandQueueAddress()), 1, 0, &d3d11Device,
			&m_d3d11DeviceContext, nullptr));

		ThrowIfFailed(d3d11Device.As(&m_d3d11On12Device));

		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_d2dFactory));
		ComPtr<IDXGIDevice> dxgiDevice;
		ThrowIfFailed(m_d3d11On12Device.As(&dxgiDevice));
		ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
		ThrowIfFailed(m_d2dDevice->CreateDeviceContext(deviceOptions, &m_d2dContext));

		float dpiX;
		float dpiY;
		m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
		m_bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

		InitializeForText();
	}

	void GraphicsCard2D::Release()
	{
	}

	ID3D11On12Device* GraphicsCard2D::GetDevice11On12()
	{
		return m_d3d11On12Device.Get();
	}

	ID3D11Resource* GraphicsCard2D::GetWrappedBackBuffer(uint32_t index)
	{
		return m_wrappedBackBuffers[index].Get();
	}

	void GraphicsCard2D::Begin()
	{
		m_d3d11On12Device->AcquireWrappedResources(m_wrappedBackBuffers[r_frameIndex].GetAddressOf(), 1);

		m_d2dContext->SetTarget(m_d2dRenderTarget[r_frameIndex].Get());
		m_d2dContext->BeginDraw();
	}

	void GraphicsCard2D::End()
	{
		ThrowIfFailed(m_d2dContext->EndDraw());

		m_d3d11On12Device->ReleaseWrappedResources(m_wrappedBackBuffers[r_frameIndex].GetAddressOf(), 1);
		m_d3d11DeviceContext->Flush();
	}

	ID2D1DeviceContext2* GraphicsCard2D::GetContext()
	{
		return m_d2dContext.Get();
	}

	void GraphicsCard2D::CreateTextFormat(IDWriteTextFormat** format)
	{
		ThrowIfFailed(m_dWriteFactory->CreateTextFormat(L"Verdana", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 50, L"en-us", format));
	}

	void GraphicsCard2D::CreateTextBrush(ID2D1SolidColorBrush** brush, const Resource::Color& color)
	{
		//ThrowIfFailed(m_d2dContext->CreateSolidColorBrush(color.ToD2DColor(), brush));
		ThrowIfFailed(m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), brush));
	}

	void GraphicsCard2D::DrawText1(const wchar_t* text, uint32_t length, IDWriteTextFormat* format, ID2D1Brush* brush)
	{
		D2D1_SIZE_F rtSize = m_d2dRenderTarget[r_frameIndex]->GetSize();
		D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
		m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
		m_d2dContext->DrawText(text, length, format, &textRect, brush);
	}
}