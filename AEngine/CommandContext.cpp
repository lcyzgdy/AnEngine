#include "CommandContext.h"

namespace AEngine::RenderCore
{
	CommandListPool * CommandListPool::GetInstance()
	{
		static CommandListPool listPool;
		return &listPool;
	}
	ID3D12GraphicsCommandList * CommandListPool::GetCommandList()
	{
		lock_guard<mutex> lock(m_mutex);
		var list = m_cp_commandListPool.front();
		m_cp_commandListPool.pop();
		return list.Get();
	}
}