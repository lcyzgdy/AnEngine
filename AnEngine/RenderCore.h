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

#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_4.h>	// For WARP
#endif

namespace AnEngine::RenderCore::Heap
{
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
	class GraphicsDevice : public Singleton<GraphicsDevice>
	{
		friend class Singleton<GraphicsDevice>;

		std::vector<std::unique_ptr<GraphicsCard>> m_graphicsCard;
		std::unique_ptr<UI::GraphicsCard2D> m_graphicsCard2D;

		GraphicsDevice();

	public:
		GraphicsCard* Default()
		{
			return m_graphicsCard[0].get();
		}
	};

	extern bool r_enableHDROutput;

	extern std::vector<std::unique_ptr<GraphicsCard>> r_graphicsCard;
	extern std::unique_ptr<UI::GraphicsCard2D> r_graphicsCard2D;
	//extern ComPtr<IDXGISwapChain3> r_swapChain_cp;
	//extern Resource::ColorBuffer* r_displayPlane[r_SwapChainBufferCount_const];
	extern uint32_t r_frameIndex;
	//extern RootSignature r_rootSignature;
#ifdef _WIN32
	extern HWND r_hwnd;
#endif // _WIN32
	//extern bool rrrr_runningFlag;
	extern std::function<void(void)> R_GetGpuError;

	void InitializeRender(HWND hwnd, int graphicCardCount = 1, bool isStable = false);

	//void CreateCommonState();

	void RenderColorBuffer(Resource::ColorBuffer* dstColorBuffer);

	void BlendBuffer(Resource::GpuResource* buffer);

	void R_Present();
}


#endif // !__RENDERCORE_H__
