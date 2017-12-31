#pragma once
#ifndef __RENDERCORE_H__
#define __RENDERCORE_H__

#include"onwind.h"
#include"DX.h"
#include<mutex>
#include<atomic>
#include"ColorBuffer.h"
#include"DescriptorHeap.hpp"
#include"RenderCoreConstants.h"
#include"GraphicCard.h"
#include"RootSignature.h"

#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_4.h>	// For WARP
#endif
using namespace Microsoft::WRL;
using namespace std;

namespace AEngine::RenderCore::Heap
{
	extern DescriptorHeapAllocator r_h_heapDescAllocator;
}

namespace AEngine::RenderCore
{
	extern bool r_enableHDROutput;

	namespace Private
	{
		extern ComPtr<IDXGIFactory4> r_cp_dxgiFactory;
	}

	extern vector<GraphicCard*> r_graphicCard;
	extern ComPtr<IDXGISwapChain3> r_cp_swapChain;
	extern Resource::ColorBuffer* r_displayPlane[r_cnt_SwapChainBufferCount];
	extern int r_frameIndex;
	extern RootSignature r_rootSignature;
#ifdef _WIN32
	extern HWND r_hwnd;
#endif // _WIN32

	void InitializeRender(HWND hwnd, int graphicCardCount = 1, bool isStable = false);

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = r_cnt_DefaultSwapChainFormat);

	void CreateCommonState();

	



	void RenderColorBuffer(Resource::ColorBuffer* colorBuffer);

	template<typename... Buffer>
	void BlendBuffer(Buffer ...)
	{
		
	}
}


#endif // !__RENDERCORE_H__
