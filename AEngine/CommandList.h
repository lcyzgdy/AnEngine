#pragma once
#ifndef __COMMANDLIST_H__
#define __COMMANDLIST_H__

#include"DX.h"

namespace AEngine::RenderCore
{
	struct CommandFormatDesc
	{
		int nodeMask;
		ID3D12CommandAllocator* allocator;
		ID3D12PipelineState* pipelineState;
	};

	class CommandList
	{
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
	public:
		CommandList() = default;

		void Create(ID3D12Device* device, CommandFormatDesc& formatDesc, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		D3D12_COMMAND_LIST_TYPE GetType();
	};
}

#endif // !__COMMANDLIST_H__
