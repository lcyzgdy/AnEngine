#include "PlatformCommandList.h"
#include "DirectX12Factory.h"

namespace AnEngine::PlatformDependence::Rendering
{
	void D3D12CommandList::Init()
	{
		ID3D12Device* device = DirectX12Factory::Instance().GetDevice();

		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_allocator));
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_allocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
	}

	void D3D12CommandList::Destroy()
	{
		ID3D12Device* device = DirectX12Factory::Instance().GetDevice();

		m_allocator.Reset();
		m_commandList.Reset();
	}
}