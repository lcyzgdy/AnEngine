#include"RenderCore.h"
#include"Screen.h"
#include"CommandContext.h"
#include"DescriptorHeap.hpp"

// 检验是否有HDR输出功能
#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

using namespace AEngine::RenderCore::Resource;
using namespace AEngine::RenderCore::Heap;

namespace AEngine::RenderCore
{
	vector<GraphicsCard*> r_graphicsCard;
	ComPtr<IDXGISwapChain3> r_cp_swapChain = nullptr;
	Resource::ColorBuffer* r_displayPlane[r_cnt_SwapChainBufferCount];
	bool r_enableHDROutput = false;
	int r_frameIndex;
#ifdef _WIN32
	HWND r_hwnd;
#endif // _WIN32

	D3D12_RESOURCE_BARRIER commonToRender;
	D3D12_RESOURCE_BARRIER renderToCommon;
	D3D12_RESOURCE_BARRIER commonToResolveDest;
	D3D12_RESOURCE_BARRIER resolveDestToCommon;
	D3D12_RESOURCE_BARRIER renderTargetToResolveDest;

	namespace Private
	{
		ComPtr<IDXGIFactory4> r_cp_dxgiFactory;
	}

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = r_cnt_DefaultSwapChainFormat);
	void InitializeCommandObject();

	void InitializeRender(HWND hwnd, int graphicCardCount, bool isStable)
	{
		r_hwnd = hwnd;
		uint32_t dxgiFactoryFlags = 0;
		// 开启Debug模式
#if defined(DEBUG) || defined(_DEBUG)
		ComPtr<ID3D12Debug> d3dDebugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
		{
			d3dDebugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(Private::r_cp_dxgiFactory.GetAddressOf()));


		while (graphicCardCount--)
		{
			GraphicsCard* aRender = new GraphicsCard();
			aRender->IsStable(isStable);
			aRender->Initialize(Private::r_cp_dxgiFactory.Get());
			r_graphicsCard.emplace_back(aRender);
		}
		//DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>* rtv = new DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>();
		//rtv->Create(r_graphicsCard[0]->GetDevice());
		DescriptorHeapAllocator::GetInstance();
		InitializeSwapChain(Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), r_hwnd);
		InitializeCommandObject();
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
		ThrowIfFailed(Private::r_cp_dxgiFactory->CreateSwapChainForHwnd(r_graphicsCard[0]->GetCommandQueue(), hwnd, &swapChainDesc,
			nullptr, nullptr, swapChain1.GetAddressOf()));

#ifdef _WIN32
		Private::r_cp_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
#endif // _WIN32

		swapChain1.As(&r_cp_swapChain);
#if !CONDITIONALLY_ENABLE_HDR_OUTPUT && defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
		{
			IDXGISwapChain4* p_swapChain = static_cast<IDXGISwapChain4*>(r_cp_swapChain.Get());
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
				r_enableHDROutput = true;
			}
		}
#endif
	}

	void InitializeCommandObject()
	{
		//DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>* rtv = new DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>(r_graphicsCard[0]->GetDevice());;
		for (uint32_t i = 0; i < r_cnt_SwapChainBufferCount; ++i)
		{
			CommandAllocator* allocator = new CommandAllocator(r_graphicsCard[0]->GetDevice());
			GraphicsCommandAllocator::GetInstance()->PushCommandAllocator(allocator);

			ComPtr<ID3D12Resource> displayPlane;
			ThrowIfFailed(r_cp_swapChain->GetBuffer(i, IID_PPV_ARGS(&displayPlane)));
			r_displayPlane[i] = new ColorBuffer(L"Primary SwapChain Buffer", displayPlane.Detach(), r_graphicsCard[0]->GetDevice(),
				DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, r_graphicsCard[0]->GetDevice()));
		}

		r_frameIndex = r_cp_swapChain->GetCurrentBackBufferIndex();

		for (int i = 0; i < 1; i++)
		{
			CommandFormatDesc desc;
			desc.allocator = GraphicsCommandAllocator::GetInstance()->GetCommandAllocator()->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(r_graphicsCard[0]->GetDevice(), desc);
			GraphicsCommandContext::GetInstance()->AddNewCommandList(list);
		}
	}

	void CreateCommonState()
	{
		commonToRender.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		commonToRender.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		commonToRender.Transition.pResource = nullptr;
		commonToRender.Transition.Subresource = 0;
		commonToRender.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		commonToRender.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		renderToCommon.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderToCommon.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		renderToCommon.Transition.pResource = nullptr;
		renderToCommon.Transition.Subresource = 0;
		renderToCommon.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		renderToCommon.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;

		commonToResolveDest.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		commonToResolveDest.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		commonToResolveDest.Transition.pResource = nullptr;
		commonToResolveDest.Transition.Subresource = 0;
		commonToResolveDest.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		commonToResolveDest.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;

		resolveDestToCommon.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resolveDestToCommon.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resolveDestToCommon.Transition.pResource = nullptr;
		resolveDestToCommon.Transition.Subresource = 0;
		resolveDestToCommon.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		resolveDestToCommon.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;

		renderTargetToResolveDest.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderTargetToResolveDest.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		renderTargetToResolveDest.Transition.pResource = nullptr;
		renderTargetToResolveDest.Transition.Subresource = 0;
		renderTargetToResolveDest.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		renderTargetToResolveDest.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
	}


	void RenderColorBuffer(ColorBuffer* colorBuffer)
	{
		var commandList = GraphicsCommandContext::GetInstance()->GetCommandList()->GetCommandList();

		commandList->ResourceBarrier(1, &commonToRender);
		commandList->ClearRenderTargetView(colorBuffer->GetRTV(), colorBuffer->GetClearColor().GetPtr(), 0, nullptr);
		commandList->ResourceBarrier(1, &renderToCommon);
		commandList->Close();
		r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(&commandList));
		r_cp_swapChain->Present(0, 0);
	}
}