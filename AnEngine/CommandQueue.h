#pragma once
#ifndef __COMMANDQUEUE_H__
#define __COMMANDQUEUE_H__


#include"DX.h"
#include<atomic>

namespace AnEngine::RenderCore
{
	class CommandQueue
	{
		friend class GraphicsCard_Obsolete;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cp_commandQueue;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_cp_fence;
		std::atomic_uint64_t m_nextFenceValue;
		std::atomic_uint64_t m_lastCompleteFenceValue;
		HANDLE m_fenceEvent;
		D3D12_COMMAND_LIST_TYPE m_type;

	public:
		// 这里要修改
		CommandQueue() = default;
		~CommandQueue() = default;

		void Initialize(ID3D12Device2* device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		void Release();

		const ID3D12CommandQueue* GetCommandQueue() const;
		ID3D12CommandQueue* GetCommandQueue();
		D3D12_COMMAND_LIST_TYPE GetType();
	};
}

#endif // !__COMMANDQUEUE_H__