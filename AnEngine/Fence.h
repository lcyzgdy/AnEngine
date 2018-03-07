#pragma once
#ifndef __FENCE_H__
#define __FENCE_H__

#include "DX.h"
//#include <functional>
#include<atomic>
#include<mutex>

namespace AnEngine::RenderCore
{
	class Fence
	{
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		//uint64_t m_fenceValue;
		ID3D12CommandQueue* m_commandQueue;
#ifdef _WIN32
		HANDLE m_fenceEvent;
#endif // _WIN32
		//uint64_t m_fenceValue;
		//uint64_t m_curFenceValue;
		std::atomic_uint64_t m_fenceValue;
		std::atomic_uint64_t m_curFenceValue;

		std::mutex m_mutex;

	public:
		explicit Fence(ID3D12CommandQueue* targetQueue);
		~Fence();

		void CpuWait(uint64_t semap);
		void CpuSignal(uint64_t semap);

		void GpuWait(uint64_t semap);
		void GpuSignal(uint64_t semap);

		void WaitForGpu();
	};
}


#endif __FENCE_H__