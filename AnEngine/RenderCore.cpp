#include"RenderCore.h"
#include"Screen.h"
#include"CommandContext.h"
#include"DescriptorHeap.hpp"
#include"Fence.h"
#include"ThreadPool.hpp"

// 检验是否有HDR输出功能
#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

using namespace AnEngine::RenderCore::Resource;
using namespace AnEngine::RenderCore::Heap;

namespace AnEngine::RenderCore
{
	vector<unique_ptr<GraphicsCard>> r_graphicsCard;
	ComPtr<IDXGISwapChain3> r_swapChain_cp = nullptr;
	Resource::ColorBuffer* r_displayPlane[r_SwapChainBufferCount_const];
	bool r_enableHDROutput = false;
	uint32_t r_frameIndex;
	uint64_t r_frameCount;

	FenceSync* r_fenceForDisplayPlane;

#ifdef _WIN32
	HWND r_hwnd;
#endif // _WIN32

	bool rrrr_runningFlag;

	namespace CommonState
	{
		D3D12_RESOURCE_BARRIER commonToRenderTarget;
		D3D12_RESOURCE_BARRIER renderTargetToCommon;
		D3D12_RESOURCE_BARRIER commonToResolveDest;
		D3D12_RESOURCE_BARRIER resolveDestToCommon;
		D3D12_RESOURCE_BARRIER renderTargetToResolveDest;
		D3D12_RESOURCE_BARRIER resolveSourceToRenderTarget;
		D3D12_RESOURCE_BARRIER presentToRenderTarget;
		D3D12_RESOURCE_BARRIER renderTargetToPresent;
		D3D12_RESOURCE_BARRIER renderTargetToResolveSource;
	}

	namespace Private
	{
		ComPtr<IDXGIFactory4> r_dxgiFactory_cp;
	}

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = r_DefaultSwapChainFormat_const);
	void InitializePipeline();
	void PopulateCommandList();

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
		rrrr_runningFlag = true;
		r_frameCount = 0;
		/*Utility::u_s_threadPool.Commit([=]
		{
			while (rrrr_runningFlag)
			{
				this_thread::sleep_for(std::chrono::milliseconds(100 / 6));
				PopulateCommandList();
			}
		});*/
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
			GraphicsCommandAllocator::GetInstance()->Push(allocator);

			ComPtr<ID3D12Resource> displayPlane;
			ThrowIfFailed(r_swapChain_cp->GetBuffer(i, IID_PPV_ARGS(&displayPlane)));
			r_displayPlane[i] = new ColorBuffer(L"Primary SwapChain Buffer", displayPlane.Detach(),
				DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		}

		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();

		for (int i = 0; i < 1; i++)
		{
			CommandFormatDesc desc;
			desc.allocator = GraphicsCommandAllocator::GetInstance()->GetOne()->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc);
			GraphicsCommandContext::GetInstance()->AddNew(list);
		}
	}

	void CreateCommonState()
	{
		using namespace CommonState;
		commonToRenderTarget.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		commonToRenderTarget.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		commonToRenderTarget.Transition.pResource = nullptr;
		commonToRenderTarget.Transition.Subresource = 0;
		commonToRenderTarget.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		commonToRenderTarget.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		renderTargetToCommon.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderTargetToCommon.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		renderTargetToCommon.Transition.pResource = nullptr;
		renderTargetToCommon.Transition.Subresource = 0;
		renderTargetToCommon.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		renderTargetToCommon.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;

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

		resolveSourceToRenderTarget.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resolveSourceToRenderTarget.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resolveSourceToRenderTarget.Transition.pResource = nullptr;
		resolveSourceToRenderTarget.Transition.Subresource = 0;
		resolveSourceToRenderTarget.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		resolveSourceToRenderTarget.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		presentToRenderTarget.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		presentToRenderTarget.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		presentToRenderTarget.Transition.pResource = nullptr;
		presentToRenderTarget.Transition.Subresource = 0;
		presentToRenderTarget.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		presentToRenderTarget.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		renderTargetToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderTargetToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		renderTargetToPresent.Transition.pResource = nullptr;
		renderTargetToPresent.Transition.Subresource = 0;
		renderTargetToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		renderTargetToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		renderTargetToResolveSource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderTargetToResolveSource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		renderTargetToResolveSource.Transition.pResource = nullptr;
		renderTargetToResolveSource.Transition.Subresource = 0;
		renderTargetToResolveSource.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		renderTargetToResolveSource.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	}

	void PopulateCommandList()
	{
		//r_graphicsCard[0]->ExecuteSync(commandLists.size(), &commandLists[0]);
		//GraphicsCommandContext::GetInstance()->PopulateFinished();
		r_fenceForDisplayPlane->Wait();
		ThrowIfFailed(r_swapChain_cp->Present(1, 0));
		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();
		r_frameCount++;
	}

	void RenderColorBuffer(ColorBuffer* dstColorBuffer)
	{
		var commandList = GraphicsCommandContext::GetInstance()->GetOne();
		var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToResolveDest;
		commonToRenderTarget.Transition.pResource = dstColorBuffer->GetResource();
		renderTargetToCommon.Transition.pResource = dstColorBuffer->GetResource();

		//iCommandList->ResourceBarrier(1, &commonToRenderTarget);
		var clearColorTemp = dstColorBuffer->GetClearColor();
		float clearColor[4] = { 0.0f, 0.0f, 0.2f, 1.0f };
		iCommandList->ClearRenderTargetView(dstColorBuffer->GetRTV(), clearColor, 0, nullptr);
		//iCommandList->ResourceBarrier(1, &renderTargetToCommon);
		iCommandList->Close();
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(_countof(ppcommandList), ppcommandList);

		GraphicsCommandContext::GetInstance()->Push(commandList);
		GraphicsCommandAllocator::GetInstance()->Push(commandAllocator);
	}

	void BlendBuffer(GpuResource* srcBuffer)
	{
		var commandList = GraphicsCommandContext::GetInstance()->GetOne();
		var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var renderTargetToResolveSrc = CommonState::renderTargetToResolveSource;
		var commonToResolveDst = CommonState::commonToResolveDest;
		var resolveSrcToRenderTarget = CommonState::resolveSourceToRenderTarget;
		var resolveDstToCommon = CommonState::resolveDestToCommon;
		renderTargetToResolveSrc.Transition.pResource = srcBuffer->GetResource();
		commonToResolveDst.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();
		resolveSrcToRenderTarget.Transition.pResource = srcBuffer->GetResource();
		resolveDstToCommon.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();

		iCommandList->ResourceBarrier(1, &renderTargetToResolveSrc);
		iCommandList->ResourceBarrier(1, &commonToResolveDst);
		iCommandList->ResolveSubresource(r_displayPlane[r_frameIndex]->GetResource(), 0, srcBuffer->GetResource(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		iCommandList->ResourceBarrier(1, &resolveDstToCommon);
		iCommandList->ResourceBarrier(1, &resolveSrcToRenderTarget);

		iCommandList->Close();
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		//r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(_countof(ppcommandList), ppcommandList);
		r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);

		r_fenceForDisplayPlane->Wait();
		ThrowIfFailed(r_swapChain_cp->Present(0, 0));

		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();

		GraphicsCommandContext::GetInstance()->Push(commandList);
		GraphicsCommandAllocator::GetInstance()->Push(commandAllocator);

		/*var commandList = GraphicsCommandContext::GetInstance()->GetCommandList();
		var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetCommandAllocator();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var commonToRenderTarget = commonToRenderTarget;
		var renderTargetToCommon = renderTargetToResolveDest;
		commonToRenderTarget.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();
		renderTargetToCommon.Transition.pResource = r_displayPlane[r_frameIndex]->GetResource();

		iCommandList->ResourceBarrier(1, &commonToRenderTarget);
		var clearColorTemp = r_displayPlane[r_frameIndex]->GetClearColor();
		float clearColor[4] = { 0, 0, 0.6, 1 };
		iCommandList->ClearRenderTargetView(r_displayPlane[r_frameIndex]->GetRTV(), clearColor, 0, nullptr);
		iCommandList->ResourceBarrier(1, &renderTargetToCommon);
		iCommandList->Close();
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(_countof(ppcommandList), ppcommandList);
		ThrowIfFailed(r_swapChain_cp->Present(1, 0));

		r_fenceForDisplayPlane->Wait();
		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();

		GraphicsCommandContext::GetInstance()->PushCommandList(commandList);
		GraphicsCommandAllocator::GetInstance()->PushCommandAllocator(commandAllocator);*/
	}
}