#include "RenderCore.h"
#include "Screen.h"
#include "Graphics/CommandContext.h"
#include "DescriptorHeap.hpp"
#include "Fence.h"
#include "ThreadPool.hpp"
#include "DTimer.h"
#include "DebugLog.h"
#include <dxgidebug.h>
#include "FenceContext.h"
#include "GpuContext.h"
#include "CommonState.h"

// 检验是否有HDR输出功能
#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

using namespace std;
using namespace Microsoft::WRL;
using namespace AnEngine::RenderCore::Resource;
using namespace AnEngine::RenderCore::Heap;
using namespace AnEngine::RenderCore::UI;
using namespace AnEngine::Debug;

namespace AnEngine::RenderCore
{
	ComPtr<IDXGIFactory7> r_dxgiFactory_cp;

	// vector<unique_ptr<GraphicsCard>> r_graphicsCard;
	unique_ptr<UI::GraphicsCard2D> r_graphicsCard2D;
	ComPtr<IDXGISwapChain3> r_swapChain_cp = nullptr;
	Resource::ColorBuffer* r_displayPlane[r_SwapChainBufferCount_const];
	bool r_enableHDROutput = false;
	uint32_t r_frameIndex;
	uint64_t r_frameCount;

	function<void(void)> R_GetGpuError = [=]()
	{
		// var hr = r_graphicsCard[0]->GetDevice()->GetDeviceRemovedReason();
		// 令(device = r_graphicsCard[0]->GetDevice());
		令(device = GpuContext::Instance().Default().GetDevice());
		var hr = device->GetDeviceRemovedReason();
	};
	/*
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
	*/
	// void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = r_DefaultSwapChainFormat_const);
	void WaitForGpu();
	void CreateCommonState();

	/*void InitializeRender(HWND hwnd, int graphicCardCount, bool isStable)
	{
		uint32_t dxgiFactoryFlags = 0;
		// 开启Debug模式
		bool debugDxgi = false;
#if defined(DEBUG) || defined(_DEBUG)

		ComPtr<ID3D12Debug3> d3dDebugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
		{
			d3dDebugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		// ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		// if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
		// {
		// 	debugDxgi = true;
		//
		// 	ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&r_dxgiFactory_cp)));
		//
		// 	dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		// 	dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		// }
#endif
		if (!debugDxgi)
		{
			ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(r_dxgiFactory_cp.GetAddressOf())), R_GetGpuError);
		}


		while (graphicCardCount--)
		{
			GraphicsCard* aRender = new GraphicsCard();
			aRender->IsStable(isStable);
			aRender->Initialize(r_dxgiFactory_cp.Get());
			r_graphicsCard.emplace_back(aRender);
		}
		r_graphicsCard2D.reset(new UI::GraphicsCard2D());
		r_graphicsCard2D->Initialize();
		DescriptorHeapAllocator::GetInstance();
		InitializeSwapChain(Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), r_hwnd);

		CreateCommonState();

		r_frameCount = 0;
	}
*/
/*const GraphicsCard& InitializeRender(IDXGIFactory6* dxgiFactory)
{
	GpuContext::Instance()->Initialize(dxgiFactory);
	return GpuContext::Instance()->Default();
}*/

/*void AttachSwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain4>& swapChain, uint32_t bufferCount)
{
	GpuContext::Instance()->AttachSwapChain(swapChain, bufferCount);
}*/
/*
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

		for (uint32_t i = 0; i < r_SwapChainBufferCount_const; ++i)
		{
			ComPtr<ID3D12Resource> displayPlane;
			ThrowIfFailed(r_swapChain_cp->GetBuffer(i, IID_PPV_ARGS(&displayPlane)));
			r_displayPlane[i] = new ColorBuffer(L"Primary SwapChain Buffer", displayPlane.Detach(),
				DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

			// 2D平面
			D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
			ThrowIfFailed(r_graphicsCard2D->GetDevice11On12()->CreateWrappedResource(r_displayPlane[i]->GetResource(),
				&d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&r_graphicsCard2D->m_wrappedBackBuffers[i])));

			ComPtr<IDXGISurface> surface;
			ThrowIfFailed(r_graphicsCard2D->m_wrappedBackBuffers[i].As(&surface));
			ThrowIfFailed(r_graphicsCard2D->m_d2dContext->CreateBitmapFromDxgiSurface(surface.Get(),
				&r_graphicsCard2D->m_bitmapProperties, &r_graphicsCard2D->m_d2dRenderTarget[i]));
		}

		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();
	}
	*/
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

	void RenderColorBuffer(ColorBuffer* dstColorBuffer)
	{
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToResolveDest;
		commonToRenderTarget.Transition.pResource = dstColorBuffer->GetResource();
		renderTargetToCommon.Transition.pResource = dstColorBuffer->GetResource();

		float clearColor[4] = { 0.0f, 0.0f, 0.2f, 1.0f };
		iCommandList->ClearRenderTargetView(dstColorBuffer->GetRtv(), clearColor, 0, nullptr);
		iCommandList->Close();
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		GpuContext::Instance().Default().GetCommandQueue()->ExecuteCommandLists(_countof(ppcommandList), ppcommandList);

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
		iList->ClearRenderTargetView(r_displayPlane[frameIndex]->GetRtv(), color, 0, nullptr);
		iList->ResolveSubresource(frame, 0, srcBuffer->GetResource(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		iList->ResourceBarrier(barrier2.size(), barrier2.begin());

		ThrowIfFailed(iList->Close(), R_GetGpuError);
#ifdef _DEBUG
		if (frameIndex != r_frameIndex)
		{
			throw exception();
		}
#endif // _DEBUG

		GraphicsContext::Push(commandList, commandAllocator);
	}

	void R_Present()
	{
		ThrowIfFailed(r_swapChain_cp->Present(0, 0), R_GetGpuError);
		r_frameIndex = r_swapChain_cp->GetCurrentBackBufferIndex();
		WaitForGpu();
	}

	void WaitForGpu()
	{
		var[fence] = FenceContext::Instance().GetOne();
		var iFence = fence->GetFence();
		uint64_t fenceValue = fence->GetFenceValue();
		fenceValue++;
		GpuContext::Instance().Default().GetCommandQueue()->Signal(iFence, fenceValue);
		fence->WaitForValue(fenceValue);
	}

}