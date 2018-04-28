#include "UICore.h"
#include "RenderCore.h"
using namespace Microsoft::WRL;

namespace AnEngine::RenderCore
{
	UICore::UICore()
	{

	}

	void UICore::Initialize()
	{
		uint32_t dxgiFactoryFlags = 0;
		uint32_t d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_d2dFactory));
		ComPtr<IDXGIDevice> dxgiDevice;
		ThrowIfFailed(r_graphicsCard[0]->m_device_cp.As(&dxgiDevice));
		ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
		ThrowIfFailed(m_d2dDevice->CreateDeviceContext(deviceOptions, &m_d2dContext));
		ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory));

		ComPtr<ID3D11Device> d3d11Device;
		ThrowIfFailed(D3D11On12CreateDevice(r_graphicsCard[0]->m_device_cp.Get(), d3d11DeviceFlags, nullptr, 0,
			reinterpret_cast<IUnknown**>(m_commandQueue.GetAddressOf()), 1, 0, &d3d11Device, &m_d3d11DeviceContext, nullptr));

		// Query the 11On12 device from the 11 device.
		ThrowIfFailed(d3d11Device.As(&m_d3d11On12Device));
	}

	void UICore::Release()
	{
	}
}