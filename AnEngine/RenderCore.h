#pragma once
#ifndef __RENDERCORE_H__
#define __RENDERCORE_H__

#include "onwind.h"
#include "DX.h"
#include <mutex>
#include <atomic>
#include "ColorBuffer.h"
#include "DescriptorHeap.hpp"
#include "RenderCoreConstants.h"
#include "GraphicsCard.h"
#include "GraphicsCard2D.h"
#include "RootSignature.h"


namespace AnEngine::RenderCore::Heap
{
	class DescriptorHeapAllocator;
	extern DescriptorHeapAllocator r_h_heapDescAllocator;
}

namespace AnEngine::RenderCore::CommonState
{
	extern D3D12_RESOURCE_BARRIER commonToRenderTarget;
	extern D3D12_RESOURCE_BARRIER renderTargetToCommon;
	extern D3D12_RESOURCE_BARRIER commonToResolveDest;
	extern D3D12_RESOURCE_BARRIER resolveDestToCommon;
	extern D3D12_RESOURCE_BARRIER renderTargetToResolveDest;
	extern D3D12_RESOURCE_BARRIER resolveSourceToRenderTarget;
	extern D3D12_RESOURCE_BARRIER presentToRenderTarget;
	extern D3D12_RESOURCE_BARRIER renderTargetToPresent;
	extern D3D12_RESOURCE_BARRIER renderTarget2ResolveSource;
	extern D3D12_RESOURCE_BARRIER depthWrite2PsResource;
	extern D3D12_RESOURCE_BARRIER psResource2DepthWrite;
}

namespace AnEngine::RenderCore
{
	extern bool r_enableHDROutput;

	// extern std::vector<std::unique_ptr<GraphicsCard>> r_graphicsCard;
	extern std::unique_ptr<UI::GraphicsCard2D> r_graphicsCard2D;
	extern uint32_t r_frameIndex;

	extern std::function<void(void)> R_GetGpuError;

	// void InitializeRender(HWND hwnd, int graphicCardCount = 1, bool isStable = false);
	// GraphicsCard* InitializeRender(IDXGIFactory6* dxgiFactory);
	// void AttachSwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain4>& swapChain, uint32_t bufferCount);

	void RenderColorBuffer(Resource::ColorBuffer* dstColorBuffer);

	void BlendBuffer(Resource::GpuResource* buffer);

	void R_Present();

}


#endif // !__RENDERCORE_H__