#include "Win32App.h"
#include <dxgidebug.h>
#include <D3Dcompiler.h>
#include "RenderCore.h"

using namespace Microsoft::WRL;
using namespace AnEngine::RenderCore;

namespace AnEngine
{
	void Win32App::Initialize()
	{
		ComPtr<ID3D12Debug3> d3dDebugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
		{
			d3dDebugController->EnableDebugLayer();
			m_dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_dxgiFactory.GetAddressOf())));

		var gCard = InitializeRender(m_dxgiFactory.Get());

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_swapBufferCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.Width = m_windowWidth;
		swapChainDesc.Height = m_windowHeight;
		swapChainDesc.Format = m_enableHdr ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;

		ComPtr<IDXGISwapChain1> swapChain1;

		ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(gCard->GetCommandQueue(),
			m_hwnd, &swapChainDesc, nullptr, nullptr, swapChain1.GetAddressOf()));
		ThrowIfFailed(swapChain1.As(&m_swapChain));


#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
		if (m_enableHdr)
		{
			IDXGISwapChain4* p_swapChain = dynamic_cast<IDXGISwapChain4*>(m_swapChain.Get());
			ComPtr<IDXGIOutput> cp_output;
			ComPtr<IDXGIOutput6> cp_output6;
			DXGI_OUTPUT_DESC1 outputDesc;
			uint32_t colorSpaceSupport;

			if (SUCCEEDED(p_swapChain->GetContainingOutput(&cp_output)) &&
				SUCCEEDED(cp_output.As(&cp_output6)) &&
				SUCCEEDED(cp_output6->GetDesc1(&outputDesc)) &&
				outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 &&
				SUCCEEDED(p_swapChain->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020, &colorSpaceSupport)) &&
				(colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) &&
				SUCCEEDED(p_swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)))
			{
			}
			else
			{
				m_enableHdr = false;
			}
		}
#else
		m_enableHdr = false;
#endif
		if (m_enableHdr)
		{
			// TODO: 支持HDR
		}

		AttachSwapChain(m_swapChain, m_swapBufferCount);
	}

	Win32App::Win32App(HWND hwnd, uint32_t windowWidth, uint32_t windowHeighht, uint32_t swapBufferCount, bool enableHdr,
		bool isStable)
		: m_hwnd(hwnd), m_windowWidth(windowWidth), m_windowHeight(windowHeighht), m_swapBufferCount(swapBufferCount),
		m_enableHdr(enableHdr), m_isStable(isStable)
	{
		Initialize();
	}
}
