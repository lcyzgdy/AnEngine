#pragma once
#ifndef __FENCE_H__
#define __FENCE_H__

#include "DX.h"
#include <functional>

namespace AEngine::RenderCore
{
	class FenceSync
	{
		ComPtr<ID3D12Fence> m_fence;
		uint64_t m_fenceValue;
		ID3D12CommandQueue* m_commandQueue;
#ifdef _WIN32
		HANDLE m_fenceEvent;
#endif // _WIN32


	public:
		explicit FenceSync(ID3D12CommandQueue* targetQueue);
		~FenceSync();

		void Wait();
	};
}


#endif __FENCE_H__