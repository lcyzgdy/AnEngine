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

		std::vector<std::shared_ptr<GraphicsCard>> m_graphicsCard;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		uint32_t m_bufferCount;

		std::vector<Resource::ColorBuffer*> m_outputBuffer;

		GpuContext() = default;

	public:
		~GpuContext();

		__forceinline GraphicsCard* Default()
		{
			return m_graphicsCard[0].get();
		}

		void Initialize(IDXGIFactory6* dxgiFactory);
		void AttachSwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain4>& swapChain, const uint32_t bufferCount);
	};
}
#endif // !__GPUCONTEXT_H__
