#include "CommandList.h"

void RenderCore::CommandList::Create(ID3D12Device * device, CommandFormatDesc & formatDesc, D3D12_COMMAND_LIST_TYPE type)
{
	ThrowIfFailed(
		device->CreateCommandList(
			formatDesc.nodeMask, type, formatDesc.allocator,
			formatDesc.pipelineState, IID_PPV_ARGS(&m_commandList)));
}

D3D12_COMMAND_LIST_TYPE RenderCore::CommandList::GetType()
{
	return m_commandList->GetType();
}
