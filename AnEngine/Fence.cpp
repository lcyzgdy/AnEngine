#include "Fence.h"
#include "RenderCore.h"
#include "DTimer.h"

namespace AnEngine::RenderCore
{
	Fence::Fence(ID3D12CommandQueue* targetQueue) : m_fenceValue(2), m_curFenceValue(0)
	{
		var device = r_graphicsCard[0]->GetDevice();
		m_commandQueue = targetQueue;
		ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
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
		//lock_guard<mutex> lock(m_mutex);
		uint64_t yyyy = m_fence->GetCompletedValue();
		//if (yyyy < semap)
		if (yyyy < m_fenceValue)
		{
			//ThrowIfFailed(m_fence->SetEventOnCompletion(semap, m_fenceEvent));
			ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}

	void Fence::CpuSignal(uint64_t semap)
	{
		ThrowIfFailed(m_fence->Signal(semap));
	}

	void Fence::GpuWait(uint64_t semap)
	{
		ThrowIfFailed(m_commandQueue->Wait(m_fence.Get(), semap));
	}

	void Fence::GpuSignal(uint64_t semap)
	{
		m_curFenceValue.store(m_fenceValue.load());
		m_fenceValue++;
		//m_commandQueue->Signal(m_fence.Get(), semap);
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
	}

	void Fence::WaitForGpu()
	{
		uint64_t fenceValue = Timer::GetTotalTicks();
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceValue));

		uint64_t yyyy = m_fence->GetCompletedValue();
		if (yyyy < fenceValue)
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}
}