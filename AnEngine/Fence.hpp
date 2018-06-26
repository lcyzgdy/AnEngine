#pragma once
#ifndef __FENCE_HPP__
#define __FENCE_HPP__

#include "DX.h"
//#include <functional>
#include<atomic>
#include<mutex>
#include"RenderCoreConstants.h"

namespace AnEngine::RenderCore
{
	class Fence
	{
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		//uint64_t m_fenceValue;
		//ID3D12CommandQueue* m_commandQueue;
#ifdef _WIN32
		// HANDLE m_fenceEvent;
		Microsoft::WRL::Wrappers::Event m_fenceEvent;
#else
		std::condition_variable m_fenceEvent;
#endif // _WIN32
		//uint64_t m_fenceValue;
		//uint64_t m_curFenceValue;
		uint64_t m_fenceValue;
		uint64_t m_curFenceValue;

		std::mutex m_mutex;

	public:
		//explicit Fence(ID3D12CommandQueue* targetQueue);
		Fence();
		~Fence();

		ID3D12Fence* GetFence();
		uint64_t GetFenceValue();

		ID3D12Fence* operator->();

		//void CpuWait(uint64_t semap);
		//void CpuSignal(uint64_t semap);

		//void GpuWait(uint64_t semap);
		//void GpuSignal(uint64_t semap);

		void WaitForGpu();
		void WaitForValue(uint64_t fenceValue);
	};
}


#endif // __FENCE_HPP__