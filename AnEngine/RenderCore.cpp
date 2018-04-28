#include"RenderCore.h"
#include"Screen.h"
#include"CommandContext.h"
#include"DescriptorHeap.hpp"
#include"Fence.hpp"
#include"ThreadPool.hpp"
#include"DTimer.h"
#include"DebugLog.h"

// 检验是否有HDR输出功能
#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

using namespace AnEngine::RenderCore::Resource;
using namespace AnEngine::RenderCore::Heap;
using namespace AnEngine::Debug;

namespace AnEngine::RenderCore
{
	ComPtr<IDXGIFactory4> r_dxgiFactory_cp;

	vector<unique_ptr<GraphicsCard>> r_graphicsCard;
	ComPtr<IDXGISwapChain3> r_swapChain_cp = nullptr;
	Resource::ColorBuffer* r_displayPlane[r_SwapChainBufferCount_const];
	bool r_enableHDROutput = false;
	uint32_t r_frameIndex;
	uint64_t r_frameCount;

	//Fence* r_fenceForDisplayPlane;
	//ComPtr<ID3D12Fence> r_fence;
	//uint64_t r_fenceValueForDisplayPlane[r_SwapChainBufferCount_const];
	//HANDLE r_fenceEvent;

	// D2D

#ifdef _WIN32
	HWND r_hwnd;
#endif // _WIN32

	//bool rrrr_runningFlag;
	function<void(void)> R_GetGpuError = [=]()
	{
		var hr = r_graphicsCard[0]->GetDevice()->GetDeviceRemovedReason();
	};

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
		D3D12_RESOURCE_BARRIER renderTarget2ResolveSource;
		D3D12_RESOURCE_BARRIER commonToResolveSource;
		D3D12_RESOURCE_BARRIER resolveSourceToCommon;
		D3D12_RESOURCE_BARRIER depthWrite2PsResource;
		D3D12_RESOURCE_BARRIER psResource2DepthWrite;
	}

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = r_DefaultSwapChainFormat_const);
	void InitializePipeline();
	void PopulateCommandList();
	void WaitForGpu();

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
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(r_dxgiFactory_cp.GetAddressOf()));


		while (graphicCardCount--)
		{
			GraphicsCard* aRender = new GraphicsCard();
			aRender->IsStable(isStable);
			aRender->Initialize(r_dxgiFactory_cp.Get(), true);
			r_graphicsCard.emplace_back(aRender);
		}
		DescriptorHeapAllocator::GetInstance();
		InitializeSwapChain(Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), r_hwnd);
		InitializePipeline();
		CreateCommonState();

		//r_fenceForDisplayPlane = new Fence(r_graphicsCard[0]->GetCommandQueue());
		/*r_graphicsCard[0]->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&r_fence));
		memset(r_fenceValueForDisplayPlane, 0, sizeof(r_fenceValueForDisplayPlane));
		r_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (r_fenceEvent == NULL)
		{
			HRESULT_FROM_WIN32(GetLastError());
		}*/
		// 帧缓冲之间的资源同步

		//rrrr_runningFlag = true;
		r_frameCount = 0;
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
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;

		ComPtr<IDXGISwapChain1> swapChain1;
		//Private::r_dxgiFactory_cp->Create
		ThrowIfFailed(r_dxgiFactory_cp->CreateSwapChainForHwnd(r_graphicsCard[0]->GetCommandQueue(), hwnd, &swapChainDesc,
			nullptr, nullptr, swapChain1.GetAddressOf()));

#ifdef _WIN32
		r_dxgiFactory_cp->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
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
		for (uint32_t i = 0; i < r_SwapChainBufferCount_const; ++i)
		{
			ComPtr<ID3D12Resource> displayPlane;
			ThrowIfFailed(r_swapChain_cp->GetBuffer(i, IID_PPV_ARGS(&displayPlane)));
			r_displayPlane[i] = new ColorBuffer(L"Primary SwapChain Buffer", displayPlane.Detach(),
				DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		}

		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();
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

		renderTarget2ResolveSource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderTarget2ResolveSource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		renderTarget2ResolveSource.Transition.pResource = nullptr;
		renderTarget2ResolveSource.Transition.Subresource = 0;
		renderTarget2ResolveSource.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		renderTarget2ResolveSource.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;

		commonToResolveSource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		commonToResolveSource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		commonToResolveSource.Transition.pResource = nullptr;
		commonToResolveSource.Transition.Subresource = 0;
		commonToResolveSource.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		commonToResolveSource.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;

		resolveSourceToCommon.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resolveSourceToCommon.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resolveSourceToCommon.Transition.pResource = nullptr;
		resolveSourceToCommon.Transition.Subresource = 0;
		resolveSourceToCommon.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		resolveSourceToCommon.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;

		depthWrite2PsResource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		depthWrite2PsResource.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		depthWrite2PsResource.Transition.pResource = nullptr;
		depthWrite2PsResource.Transition.Subresource = 0;
		depthWrite2PsResource.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		depthWrite2PsResource.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		psResource2DepthWrite.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		psResource2DepthWrite.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		psResource2DepthWrite.Transition.pResource = nullptr;
		psResource2DepthWrite.Transition.Subresource = 0;
		psResource2DepthWrite.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		psResource2DepthWrite.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}

	/*void PopulateCommandList()
	{
		//r_fenceForDisplayPlane->CpuWait(r_fenceValueForDisplayPlane[r_frameIndex]);
		ThrowIfFailed(r_swapChain_cp->Present(1, 0));
		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();
		r_frameCount++;
		r_fenceValueForDisplayPlane[r_frameIndex] = Timer::GetTotalTicks();
	}*/

	void RenderColorBuffer(ColorBuffer* dstColorBuffer)
	{
		//var commandList = GraphicsCommandContext::GetInstance()->GetOne();
		//var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToResolveDest;
		commonToRenderTarget.Transition.pResource = dstColorBuffer->GetResource();
		renderTargetToCommon.Transition.pResource = dstColorBuffer->GetResource();

		//iList->ResourceBarrier(1, &commonToRenderTarget);
		var clearColorTemp = dstColorBuffer->GetClearColor();
		float clearColor[4] = { 0.0f, 0.0f, 0.2f, 1.0f };
		iCommandList->ClearRenderTargetView(dstColorBuffer->GetRTV(), clearColor, 0, nullptr);
		//iList->ResourceBarrier(1, &renderTargetToCommon);
		iCommandList->Close();
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		r_graphicsCard[0]->GetCommandQueue()->ExecuteCommandLists(_countof(ppcommandList), ppcommandList);

		//GraphicsCommandContext::GetInstance()->Push(commandList);
		//GraphicsCommandAllocator::GetInstance()->Push(commandAllocator);
		GraphicsContext::Push(commandList, commandAllocator);
	}

	void BlendBuffer(GpuResource* srcBuffer)
	{
		uint32_t frameIndex = r_frameIndex;
		var frame = r_displayPlane[frameIndex]->GetResource();

		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iList->Reset(iCommandAllocator, nullptr));

		var commonToResolveSrc = CommonState::commonToResolveSource;
		var commonToResolveDst = CommonState::commonToResolveDest;
		var resolveSrcToCommon = CommonState::resolveSourceToCommon;
		var resolveDstToCommon = CommonState::resolveDestToCommon;
		commonToResolveSrc.Transition.pResource = srcBuffer->GetResource();
		commonToResolveDst.Transition.pResource = frame;
		resolveSrcToCommon.Transition.pResource = srcBuffer->GetResource();
		resolveDstToCommon.Transition.pResource = frame;
		var barrier1 = { commonToResolveSrc, commonToResolveDst };
		var barrier2 = { resolveSrcToCommon ,resolveDstToCommon };

		iList->ResourceBarrier(barrier1.size(), barrier1.begin());
		float color[4] = { 0, 0, 0, 1 };
		iList->ClearRenderTargetView(r_displayPlane[frameIndex]->GetRTV(), color, 0, nullptr);
		iList->ResolveSubresource(frame, 0, srcBuffer->GetResource(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		iList->ResourceBarrier(barrier2.size(), barrier2.begin());

		ThrowIfFailed(iList->Close(), R_GetGpuError);
		//ID3D12CommandList* ppcommandList[] = { iList };
#ifdef _DEBUG
		if (frameIndex != r_frameIndex)
		{
			throw exception();
		}
#endif // _DEBUG
		//r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);

		//WaitForGpu();
		GraphicsContext::Push(commandList, commandAllocator);

		ThrowIfFailed(r_swapChain_cp->Present(0, 0), R_GetGpuError);
		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();
		WaitForGpu();
	}

	void WaitForGpu()
	{
		/*uint64_t currentFenceValue = r_fenceValueForDisplayPlane[r_frameIndex];
		var commandQueue = r_graphicsCard[0]->GetCommandQueue();
		ThrowIfFailed(commandQueue->Signal(r_fence.Get(), currentFenceValue));
		// 在队列中调度信号命令。

		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();
		r_frameCount++;
		// 更新帧编号

		if (r_fence->GetCompletedValue() < r_fenceValueForDisplayPlane[r_frameIndex])
		{
			r_fence->SetEventOnCompletion(r_fenceValueForDisplayPlane[r_frameIndex], r_fenceEvent);
			WaitForSingleObjectEx(r_fenceEvent, INFINITE, false);
		}// 如果下一帧还没有渲染完，则等待

		r_fenceValueForDisplayPlane[r_frameIndex] = currentFenceValue + 1LL;*/

		var[fence] = FenceContext::GetInstance()->GetOne();
		var iFence = fence->GetFence();
		uint64_t fenceValue = fence->GetFenceValue();
		fenceValue++;
		r_graphicsCard[0]->GetCommandQueue()->Signal(iFence, fenceValue);
		fence->WaitForValue(fenceValue);
	}
}