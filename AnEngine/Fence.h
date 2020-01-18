#pragma once
#ifndef __FENCE_H__
#define __FENCE_H__

#include "DX.h"
#include<atomic>
#include<mutex>
#include"RenderCoreConstants.h"

namespace AnEngine::RenderCore
{
	class Fence
	{
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
#ifdef _WIN32
		// HANDLE m_fenceEvent;
		Microsoft::WRL::Wrappers::Event m_fenceEvent;
#else
		std::condition_variable m_fenceEvent;
#endif // _WIN32
		uint64_t m_fenceValue;
		uint64_t m_completeValue;

		std::mutex m_mutex;

	public:
		Fence();
		~Fence();

		ID3D12Fence* GetFence() { return m_fence.Get(); }
		uint64_t GetFenceValue() { return m_fenceValue; }

		void WaitForGpu();
		void WaitForValue(uint64_t fenceValue);

		void SetCompeteValue(uint64_t fenceValue) { m_fenceValue = fenceValue; }
		bool Complete() { return m_fence->GetCompletedValue() >= m_fenceValue; }
	};
}


#endif // __FENCE_H__