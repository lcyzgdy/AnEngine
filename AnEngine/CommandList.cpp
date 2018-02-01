#include "CommandList.h"
#include "RenderCore.h"

namespace AnEngine::RenderCore
{
	CommandList::CommandList(CommandFormatDesc& formatDesc, D3D12_COMMAND_LIST_TYPE type) : m_desc(formatDesc)
	{
		ID3D12Device* device = r_graphicsCard[0]->GetDevice();
		ThrowIfFailed(device->CreateCommandList(formatDesc.nodeMask, type, formatDesc.allocator,
			formatDesc.pipelineState, IID_PPV_ARGS(&m_commandList)));
		m_commandList->Close();
	}

	CommandList::~CommandList()
	{
	}

	D3D12_COMMAND_LIST_TYPE CommandList::GetType()
	{
		return m_commandList->GetType();
	}

	ID3D12GraphicsCommandList* CommandList::GetCommandList()
	{
		return m_commandList.Get();
	}
	CommandFormatDesc CommandList::GetDesc()
	{
		return m_desc;
	}
}


namespace AnEngine::RenderCore
{
	CommandAllocator::CommandAllocator(D3D12_COMMAND_LIST_TYPE type) : m_type(type)
	{
		ID3D12Device* device = r_graphicsCard[0]->GetDevice();
		if (!SUCCEEDED(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_allocator))))
		{
			var hr = device->GetDeviceRemovedReason();
			throw std::exception();
		}
	}

	D3D12_COMMAND_LIST_TYPE CommandAllocator::GetGype()
	{
		return m_type;
	}

	ID3D12CommandAllocator* CommandAllocator::GetAllocator()
	{
		return m_allocator.Get();
	}
}
