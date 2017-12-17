#include "CommandContext.h"

namespace AEngine::RenderCore
{
	GraphicsCommandListPool::GraphicsCommandListPool()
	{
		
	}

	GraphicsCommandListPool * GraphicsCommandListPool::GetInstance()
	{
		static GraphicsCommandListPool listPool;
		return &listPool;
	}
	ID3D12GraphicsCommandList * GraphicsCommandListPool::GetCommandList()
	{
		lock_guard<mutex> lock(m_mutex);
		var list = m_cp_commandListPool.front();
		m_cp_commandListPool.pop();
		return const_cast<ID3D12GraphicsCommandList*>(list.Get());
	}

	void GraphicsCommandListPool::PushCommandList(ID3D12GraphicsCommandList * list)
	{
		lock_guard<std::mutex> lock(m_mutex);
		m_cp_commandListPool.emplace(list);
	}
}