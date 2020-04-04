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

namespace AnEngine::RenderCore
{
	extern bool r_enableHDROutput;

	extern std::unique_ptr<UI::GraphicsCard2D> r_graphicsCard2D;
	extern uint32_t r_frameIndex;

	extern std::function<void(void)> R_GetGpuError;


	void RenderColorBuffer(Resource::ColorBuffer* dstColorBuffer);

	void BlendBuffer(Resource::GpuResource* buffer);

	void R_Present();

}


#endif // !__RENDERCORE_H__