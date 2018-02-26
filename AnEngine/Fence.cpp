#include "Fence.h"
#include "RenderCore.h"
#include "DTimer.h"

namespace AnEngine::RenderCore
{
	Fence::Fence(ID3D12CommandQueue* targetQueue) //: m_fenceValue(0)
	{
		var device = r_graphicsCard[0]->GetDevice();
		m_commandQueue = targetQueue;
		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
#ifdef _WIN32
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
#endif // _WIN32
	}

	Fence::~Fence()
	{
#ifdef _WIN32
		CloseHandle(m_fenceEvent);
#endif // _WIN32
	}

	void Fence::CpuWait(uint64_t semap)
	{
		//const uint64_t fenceValue = m_fenceValue;
		//const uint64_t fenceValue = DTimer::GetInstance()->GetTotalTicks();
		//ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceValue));
		//m_fenceValue++;
		int yyyy = m_fence->GetCompletedValue();
		if (yyyy < semap)
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(semap, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}

	void Fence::CpuSignal(uint64_t semap)
	{
		m_fence->Signal(semap);
	}

	void Fence::GpuWait(uint64_t semap)
	{
		m_commandQueue->Wait(m_fence.Get(), semap);
	}

	void Fence::GpuSignal(uint64_t semap)
	{
		m_commandQueue->Signal(m_fence.Get(), semap);
	}

	void Fence::WaitForGpu()
	{
		uint64_t fenceValue = Timer::GetTotalTicks();
		m_commandQueue->Signal(m_fence.Get(), fenceValue);

		int yyyy = m_fence->GetCompletedValue();
		if (yyyy < fenceValue)
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}
}