#include"RenderCore.h"
#include"Screen.h"
#include"CommandContext.h"
#include"DescriptorHeap.hpp"
#include"Fence.h"

// 检验是否有HDR输出功能
#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

using namespace AEngine::RenderCore::Resource;
using namespace AEngine::RenderCore::Heap;

namespace AEngine::RenderCore
{
	vector<GraphicsCard*> r_graphicsCard;
	ComPtr<IDXGISwapChain3> r_swapChain_cp = nullptr;
	Resource::ColorBuffer* r_displayPlane[r_SwapChainBufferCount_const];
	bool r_enableHDROutput = false;
	uint32_t r_frameIndex;

	FenceSync* r_fenceForDisplayPlane;

#ifdef _WIN32
	HWND r_hwnd;
#endif // _WIN32

	D3D12_RESOURCE_BARRIER r_commonToRenderTarget;
	D3D12_RESOURCE_BARRIER r_renderTargetToCommon;
	D3D12_RESOURCE_BARRIER r_commonToResolveDest;
	D3D12_RESOURCE_BARRIER r_resolveDestToCommon;
	D3D12_RESOURCE_BARRIER r_renderTargetToResolveDest;
	D3D12_RESOURCE_BARRIER r_resolveSourceToRenderTarget;
	D3D12_RESOURCE_BARRIER r_presentToRenderTarget;
	D3D12_RESOURCE_BARRIER r_renderTargetToPresent;

	namespace Private
	{
		ComPtr<IDXGIFactory4> r_dxgiFactory_cp;
	}

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = r_DefaultSwapChainFormat_const);
	void InitializePipeline();

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
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(Private::r_dxgiFactory_cp.GetAddressOf()));


		while (graphicCardCount--)
		{
			GraphicsCard* aRender = new GraphicsCard();
			aRender->IsStable(isStable);
			aRender->Initialize(Private::r_dxgiFactory_cp.Get());
			r_graphicsCard.emplace_back(aRender);
		}
		//DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>* rtv = new DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>();
		//rtv->Create(r_graphicsCard[0]->GetDevice());
		DescriptorHeapAllocator::GetInstance();
		InitializeSwapChain(Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), r_hwnd);
		InitializePipeline();
		CreateCommonState();

		r_fenceForDisplayPlane = new FenceSync(r_graphicsCard[0]->GetCommandQueue());
	}

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = r_SwapChainBufferCount_const;
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
		//Private::r_dxgiFactory_cp->Create
		ThrowIfFailed(Private::r_dxgiFactory_cp->CreateSwapChainForHwnd(r_graphicsCard[0]->GetCommandQueue(), hwnd, &swapChainDesc,
			nullptr, nullptr, swapChain1.GetAddressOf()));

#ifdef _WIN32
		Private::r_dxgiFactory_cp->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
#endif // _WIN32

		ThrowIfFailed(swapChain1.As(&r_swapChain_cp));
#if !CONDITIONALLY_ENABLE_HDR_OUTPUT && defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
		{
			IDXGISwapChain4* p_swapChain = static_cast<IDXGISwapChain4*>(r_swapChain_cp.Get());
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

	void InitializePipeline()
	{
		//DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>* rtv = new DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>(r_graphicsCard[0]->GetDevice());;
		for (uint32_t i = 0; i < r_SwapChainBufferCount_const; ++i)
		{
			CommandAllocator* allocator = new CommandAllocator();
			GraphicsCommandAllocator::GetInstance()->PushCommandAllocator(allocator);

			ComPtr<ID3D12Resource> displayPlane;
			ThrowIfFailed(r_swapChain_cp->GetBuffer(i, IID_PPV_ARGS(&displayPlane)));
			r_displayPlane[i] = new ColorBuffer(L"Primary SwapChain Buffer", displayPlane.Detach(),
				DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		}

		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();

		for (int i = 0; i < 1; i++)
		{
			CommandFormatDesc desc;
			desc.allocator = GraphicsCommandAllocator::GetInstance()->GetCommandAllocator()->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc);
			GraphicsCommandContext::GetInstance()->AddNewCommandList(list);
		}
	}

	void CreateCommonState()
	{
		r_commonToRenderTarget.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_commonToRenderTarget.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_commonToRenderTarget.Transition.pResource = nullptr;
		r_commonToRenderTarget.Transition.Subresource = 0xffffffff;
		r_commonToRenderTarget.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		r_commonToRenderTarget.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		r_renderTargetToCommon.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_renderTargetToCommon.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_renderTargetToCommon.Transition.pResource = nullptr;
		r_renderTargetToCommon.Transition.Subresource = 0xffffffff;
		r_renderTargetToCommon.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		r_renderTargetToCommon.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;

		r_commonToResolveDest.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_commonToResolveDest.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_commonToResolveDest.Transition.pResource = nullptr;
		r_commonToResolveDest.Transition.Subresource = 0xffffffff;
		r_commonToResolveDest.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		r_commonToResolveDest.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;

		r_resolveDestToCommon.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_resolveDestToCommon.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_resolveDestToCommon.Transition.pResource = nullptr;
		r_resolveDestToCommon.Transition.Subresource = 0xffffffff;
		r_resolveDestToCommon.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		r_resolveDestToCommon.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;

		r_renderTargetToResolveDest.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_renderTargetToResolveDest.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_renderTargetToResolveDest.Transition.pResource = nullptr;
		r_renderTargetToResolveDest.Transition.Subresource = 0xffffffff;
		r_renderTargetToResolveDest.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		r_renderTargetToResolveDest.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;

		r_resolveSourceToRenderTarget.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_resolveSourceToRenderTarget.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_resolveSourceToRenderTarget.Transition.pResource = nullptr;
		r_resolveSourceToRenderTarget.Transition.Subresource = 0xffffffff;
		r_resolveSourceToRenderTarget.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		r_resolveSourceToRenderTarget.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		r_presentToRenderTarget.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_presentToRenderTarget.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_presentToRenderTarget.Transition.pResource = nullptr;
		r_presentToRenderTarget.Transition.Subresource = 0xffffffff;
		r_presentToRenderTarget.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		r_presentToRenderTarget.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		r_renderTargetToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		r_renderTargetToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		r_renderTargetToPresent.Transition.pResource = nullptr;
		r_renderTargetToPresent.Transition.Subresource = 0xffffffff;
		r_renderTargetToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		r_renderTargetToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	}

	void RenderColorBuffer(ColorBuffer* destColorBuffer)
	{
		var pCommandList = GraphicsCommandContext::GetInstance()->GetCommandList();
		var commandList = pCommandList->GetCommandList();

		var commonToRenderTarget = r_commonToRenderTarget;
		var renderTargetToCommon = r_renderTargetToResolveDest;
		commonToRenderTarget.Transition.pResource = destColorBuffer->GetResource();
		renderTargetToCommon.Transition.pResource = destColorBuffer->GetResource();

		commandList->ResourceBarrier(1, &commonToRenderTarget);
		var clearColorTemp = destColorBuffer->GetClearColor();
		float clearColor[4] = { clearColorTemp.R(), clearColorTemp.G(), clearColorTemp.B(), clearColorTemp.A() };
		commandList->ClearRenderTargetView(destColorBuffer->GetRTV(), clearColor, 0, nullptr);
		commandList->ResourceBarrier(1, &renderTargetToCommon);
		commandList->Close();
		r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(&commandList));
		//r_swapChain_cp->Present(0, 0);

		GraphicsCommandContext::GetInstance()->PushCommandList(pCommandList);
	}

	void BlendBuffer(GpuResource* srcBuffer)
	{
		/*var commandList = GraphicsCommandContext::GetInstance()->GetCommandList();
		var iCommandList = commandList->GetCommandList();

		var renderTargetToResolveDest = r_renderTargetToResolveDest;
		var commonToResolveDest = r_commonToResolveDest;
		var resolveDestToCommon = r_resolveDestToCommon;
		renderTargetToResolveDest.Transition.pResource = srcBuffer->GetResource();
		commonToResolveDest.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();
		resolveDestToCommon.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();

		iCommandList->ResourceBarrier(1, &r_renderTargetToResolveDest);
		iCommandList->ResolveSubresource(r_displayPlane[r_frameIndex]->GetResource(), 0, srcBuffer->GetResource(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		iCommandList->ResourceBarrier(1, &r_resolveDestToCommon);
		//resolveDestTo

		iCommandList->Close();
		r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(&iCommandList));
		r_swapChain_cp->Present(1, 0);
		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();

		GraphicsCommandContext::GetInstance()->PushCommandList(commandList);*/

		var commandList = GraphicsCommandContext::GetInstance()->GetCommandList();
		var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetCommandAllocator();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var commonToRenderTarget = r_commonToRenderTarget;
		var renderTargetToCommon = r_renderTargetToResolveDest;
		commonToRenderTarget.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();
		renderTargetToCommon.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();

		iCommandList->ResourceBarrier(1, &commonToRenderTarget);
		var clearColorTemp = r_displayPlane[r_frameIndex]->GetClearColor();
		float clearColor[4] = { 0, 0, 1, 1 };
		iCommandList->ClearRenderTargetView(r_displayPlane[r_frameIndex]->GetRTV(), clearColor, 0, nullptr);
		iCommandList->ResourceBarrier(1, &renderTargetToCommon);
		iCommandList->Close();
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(_countof(ppcommandList), ppcommandList);
		ThrowIfFailed(r_swapChain_cp->Present(1, 0));

		r_fenceForDisplayPlane->Wait();
		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();

		GraphicsCommandContext::GetInstance()->PushCommandList(commandList);
		GraphicsCommandAllocator::GetInstance()->PushCommandAllocator(commandAllocator);
	}
		}