#include "Fence.h"
#include "RenderCore.h"
#include "DTimer.h"
#include "GpuContext.h"

namespace AnEngine::RenderCore
{
	Fence::Fence() : m_fenceValue(0)
	{
		ID3D12Device* device = GpuContext::Instance()->Default();
		ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
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