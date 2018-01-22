#include "Fence.h"
#include "RenderCore.h"

namespace AnEngine::RenderCore
{
	FenceSync::FenceSync(ID3D12CommandQueue* targetQueue) : m_fenceValue(0)
	{
		var device = r_graphicsCard[0]->GetDevice();
		m_commandQueue = targetQueue;
		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
#ifdef _WIN32
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
#endif // _WIN32
	}

	FenceSync::~FenceSync()
	{
#ifdef _WIN32
		CloseHandle(m_fenceEvent);
#endif // _WIN32

	}

	void FenceSync::Wait()
	{
		const uint64_t fenceValue = m_fenceValue;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceValue));
		m_fenceValue++;
		int yyyy = m_fence->GetCompletedValue();
		if (m_fence->GetCompletedValue() < fenceValue)
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}
}