#include "CommandList.h"

/*void AEngine::RenderCore::CommandList::Create(ID3D12Device * device, CommandFormatDesc & formatDesc, D3D12_COMMAND_LIST_TYPE type)
{

}*/
namespace AEngine::RenderCore
{
	CommandList::CommandList(ID3D12Device * device, CommandFormatDesc& formatDesc, D3D12_COMMAND_LIST_TYPE type) : m_desc(formatDesc)
	{
		ThrowIfFailed(device->CreateCommandList(formatDesc.nodeMask, type, formatDesc.allocator,
			formatDesc.pipelineState, IID_PPV_ARGS(&m_commandList)));
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


namespace AEngine::RenderCore
{
	CommandAllocator::CommandAllocator(ID3D12Device * device, D3D12_COMMAND_LIST_TYPE type) : m_type(type)
	{
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

	ID3D12CommandAllocator * CommandAllocator::GetAllocator()
	{
		return m_allocator.Get();
	}
}
