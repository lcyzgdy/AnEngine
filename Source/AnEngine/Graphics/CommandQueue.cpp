#include"CommandQueue.h"

namespace AnEngine::RenderCore
{
	void CommandQueue::Initialize(ID3D12Device2* p_device, D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = type;
		p_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_cp_commandQueue.GetAddressOf()));
		m_type = type;
	}

	void CommandQueue::Release()
	{
		CloseHandle(m_fenceEvent);
	}

	const ID3D12CommandQueue* CommandQueue::GetCommandQueue() const
	{
		return m_cp_commandQueue.Get();
	}

	ID3D12CommandQueue* CommandQueue::GetCommandQueue()
	{
		return m_cp_commandQueue.Get();
	}

	D3D12_COMMAND_LIST_TYPE CommandQueue::GetType()
	{
		return m_type;
	}
}