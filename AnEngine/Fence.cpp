#include "Fence.h"
#include "RenderCore.h"
#include "DTimer.h"

namespace AnEngine::RenderCore
{
	Fence::Fence() : m_fenceValue(0)
	{
		var device = r_graphicsCard[0]->GetDevice();
		ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
#ifdef _WIN32
		m_fenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
		if (!m_fenceEvent.IsValid())
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
#endif // _WIN32
	}

	Fence::~Fence()
	{
#ifdef _WIN32
		//CloseHandle(m_fenceEvent);
		m_fenceEvent.Close();
#endif // _WIN32
	}

	/*
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
	*/
	void Fence::WaitForGpu()
	{
		if (m_fence->GetCompletedValue() < m_fenceValue)
		{
			m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent.Get());
			WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, false);
		}
	}

	void Fence::WaitForValue(uint64_t fenceValue)
	{
		if (m_fence->GetCompletedValue() < fenceValue)
		{
			m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get());
			WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, false);
		}
		m_fenceValue = fenceValue;
	}
}