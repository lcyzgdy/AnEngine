#pragma once
#ifndef __GPUCONTEXT_H__
#define __GPUCONTEXT_H__

#include "onwind.h"
#include "GraphicsCard.h"
#include "ColorBuffer.h"

namespace AnEngine::RenderCore
{
	// 管理所有的 device、adapter
	class GpuContext : public Singleton<GpuContext>
	{
		friend class Singleton<GpuContext>;
		friend class Engine;

		// std::vector<std::shared_ptr<GraphicsCard>> m_graphicsCard;
		std::vector<GraphicsCard> m_graphicsCard;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		std::vector<Resource::ColorBuffer*> m_outputBuffer;
		uint32_t m_bufferCount;
		uint32_t m_frameIndex;


		GpuContext() = default;


		void Initialize(IDXGIFactory6* dxgiFactory);
		void AttachSwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain4>& swapChain, const uint32_t bufferCount);

		~GpuContext();

	public:
		__forceinline const GraphicsCard& Default()
		{
			return m_graphicsCard[0];
		}
	};
}
#endif // !__GPUCONTEXT_H__
