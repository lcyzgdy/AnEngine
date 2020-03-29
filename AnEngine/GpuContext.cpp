#include "GpuContext.h"
#include "DescriptorHeap.hpp"

namespace AnEngine::RenderCore
{
	GpuContext::~GpuContext()
	{
		for (int i = 0; i < m_outputBuffer.size(); i++)
		{
			delete m_outputBuffer[i];
		}
	}

	void GpuContext::Initialize(IDXGIFactory6* dxgiFactory)
	{
		var card = std::make_shared<GraphicsCard>();
		card->Initialize(dxgiFactory);
		m_graphicsCard.emplace_back(card);
	}

	void GpuContext::AttachSwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain4>& swapChain, const uint32_t bufferCount)
	{
		swapChain.CopyTo(m_swapChain.GetAddressOf());
		m_bufferCount = bufferCount;

		for (int i = 0; i < m_bufferCount; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> displayPlane;
			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&displayPlane)));
			var displayBuffer = new Resource::ColorBuffer(L"Primary SwapChain Buffer", displayPlane.Detach(),
				DescriptorHeapAllocator::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
			m_outputBuffer.push_back(displayBuffer);
		}
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
}