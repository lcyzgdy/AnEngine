#include "CommandContext.h"

namespace AEngine::RenderCore
{
	GraphicsCommandContext::GraphicsCommandContext()
	{

	}

	GraphicsCommandContext * GraphicsCommandContext::GetInstance()
	{
		static GraphicsCommandContext listPool;
		return &listPool;
	}

	CommandList * GraphicsCommandContext::GetCommandList()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		while (m_commandListPool.size() <= 0);
		lock_guard<mutex> lock(m_mutex);
		var list = m_commandListPool.front();
		m_commandListPool.pop();
		return list;
	}

	void GraphicsCommandContext::PushCommandList(CommandList * list)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		lock_guard<std::mutex> lock(m_mutex);
		m_commandListPool.emplace(list);
	}

	void GraphicsCommandContext::AddNewCommandList()
	{
		CommandList* list = new CommandList();
		this->PushCommandList(list);
	}
}