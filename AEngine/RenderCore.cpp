#include"RenderCore.h"
#include"Screen.h"

// 检验是否有HDR输出功能
#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

namespace AEngine::RenderCore
{
	vector<GraphicCard*> r_renderCore;
	ComPtr<IDXGISwapChain3> r_cp_swapChain = nullptr;
	Resource::ColorBuffer r_displayPlane[r_cnt_SwapChainBufferCount];
	bool r_enableHDROutput = false;
	int r_frameIndex;
#ifdef _WIN32
	HWND r_hwnd;
#endif // _WIN32

	namespace Private
	{
		ComPtr<IDXGIFactory4> r_cp_dxgiFactory;
	}

	void InitializeRender(HWND hwnd, int graphicCardCount, bool isStable)
	{
		r_hwnd = hwnd;
		while (graphicCardCount--)
		{
			GraphicCard* aRender = new GraphicCard();
			aRender->IsStable(isStable);
			aRender->Initialize(Private::r_cp_dxgiFactory.Get());
			r_renderCore.emplace_back(aRender);
		}
		InitializeSwapChain(Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), r_hwnd);
		CreateCommonState();

	}

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = r_cnt_SwapChainBufferCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = dxgiFormat;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;

		ComPtr<IDXGISwapChain1> swapChain1;
		//Private::r_cp_dxgiFactory->Create
		ThrowIfFailed(Private::r_cp_dxgiFactory->CreateSwapChainForHwnd
		(
			const_cast<ID3D12CommandQueue*>(r_renderCore[0]->GetCommandQueue()),
			hwnd, &swapChainDesc, nullptr, nullptr, swapChain1.GetAddressOf()
		));
		swapChain1.As(&r_cp_swapChain);
#if CONDITIONALLY_ENABLE_HDR_OUTPUT && defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
		{
			IDXGISwapChain4* p_swapChain = static_cast<IDXGISwapChain4*>(r_cp_swapChain.Get());
			ComPtr<IDXGIOutput> cp_output;
			ComPtr<IDXGIOutput6> cp_output6;
			DXGI_OUTPUT_DESC1 outputDesc;
			UINT colorSpaceSupport;

			if (SUCCEEDED(p_swapChain->GetContainingOutput(&cp_output)) &&
				SUCCEEDED(cp_output.As(&cp_output6)) &&
				SUCCEEDED(cp_output6->GetDesc1(&outputDesc)) &&
				outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 &&
				SUCCEEDED(p_swapChain->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020, &colorSpaceSupport)) &&
				(colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) &&
				SUCCEEDED(p_swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)))
			{
				r_enableHDROutput = true;
			}
		}
#endif
		for (UINT i = 0; i < r_cnt_SwapChainBufferCount; ++i)
		{
			ComPtr<ID3D12Resource> cp_displayPlane;
			ThrowIfFailed(r_cp_swapChain->GetBuffer(i, IID_PPV_ARGS(&cp_displayPlane)));
			r_displayPlane[i].CreateFromSwapChain(L"Primary SwapChain Buffer",
				cp_displayPlane.Detach(), r_renderCore[0]->GetDevice(),
				&RenderCore::Heap::r_h_heapDescAllocator);
		}
#ifdef _WIN32
		Private::r_cp_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
#endif // _WIN32

		r_frameIndex = r_cp_swapChain->GetCurrentBackBufferIndex();
	}

	void CreateCommonState()
	{
		return;
	}
}