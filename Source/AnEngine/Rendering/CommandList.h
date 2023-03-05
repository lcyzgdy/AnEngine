#pragma once
#ifndef __ANENGINE_COMMANDLIST_H__
#define __ANENGINE_COMMANDLIST_H__

#include "../onwind.h"

namespace AnEngine::RenderCore
{
	struct CommandFormatDesc
	{
		int nodeMask;
		ID3D12CommandAllocator* allocator;
		ID3D12PipelineState* pipelineState;
	};

	class CommandList
	{
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		CommandFormatDesc m_desc;

	public:
		explicit CommandList(CommandFormatDesc& formatDesc, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		~CommandList();

		//void Create(ID3D12Device* device, CommandFormatDesc& formatDesc, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		D3D12_COMMAND_LIST_TYPE GetType();
		ID3D12GraphicsCommandList* GetCommandList();
		CommandFormatDesc GetDesc();
	};

	class CommandAllocator
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
		D3D12_COMMAND_LIST_TYPE m_type;
	public:
		explicit CommandAllocator(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		~CommandAllocator() = default;

		D3D12_COMMAND_LIST_TYPE GetGype();
		ID3D12CommandAllocator* GetAllocator();
	};
}

#ifdef _WIN32
#include "../PlatformDependence/Windows/DirectX/PlatformCommandList.h"
#endif

namespace AnEngine::Rendering
{
	class IPlatformCommandListImpl
	{
	public:
		virtual void Init() = 0;
		virtual void Destroy() = 0;
	};

	class CommandList : IPlatformCommandListImpl
	{
		std::shared_ptr<IPlatformCommandListImpl> m_impl;
	public:
		CommandList() = default;
		~CommandList() = default;

		void Init() override;
		void Destroy() override;
	};
}

#endif // !__ANENGINE_COMMANDLIST_H__
