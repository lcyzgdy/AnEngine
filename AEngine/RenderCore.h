#pragma once
#ifndef __RENDERCORE_H__
#define __RENDERCORE_H__

#include"onwind.h"
#include"DX.h"
#include<dxgi1_6.h>
#include<mutex>
#include<atomic>
#include"ColorBuffer.h"
#include"DescriptorHeap.h"
#include"RenderCoreConstants.h"
#include"GraphicCard.h"
using namespace Microsoft::WRL;
using namespace std;

namespace AEngine::RenderCore::Heap
{
	extern DescriptorAllocator r_h_heapDescAllocator;
}

namespace AEngine::RenderCore
{
	extern bool r_enableHDROutput;

	namespace Private
	{
		extern ComPtr<IDXGIFactory4> r_cp_dxgiFactory;
	}

	extern vector<GraphicCard*> r_renderCore;
	extern ComPtr<IDXGISwapChain3> r_cp_swapChain;
	extern Resource::ColorBuffer r_displayPlane[r_cnt_SwapChainBufferCount];
	extern int r_frameIndex;
#ifdef _WIN32
	extern HWND r_hwnd;
#endif // _WIN32

	void InitializeRender(HWND hwnd, int graphicCardCount = 1, bool isStable = false);

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = r_cnt_DefaultSwapChainFormat);

	void CreateCommonState();
}


#endif // !__RENDERCORE_H__
