#include "CommandContext.h"

namespace AEngine::RenderCore
{
	GraphicsCommandContext* GraphicsCommandContext::m_uniqueObj;

	GraphicsCommandContext::GraphicsCommandContext()
	{

	}

	GraphicsCommandContext::~GraphicsCommandContext()
	{
		while (!m_commandListPool.empty())
		{
			var list = m_commandListPool.front();
			delete list;
			m_commandListPool.pop();
		}
	}

	GraphicsCommandContext * GraphicsCommandContext::GetInstance()
	{
		if (m_uniqueObj == nullptr)
		{
			m_uniqueObj = new GraphicsCommandContext();
		}
		return m_uniqueObj;
	}

	CommandList* GraphicsCommandContext::GetCommandList()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		//while (m_commandListPool.size() <= 0) this_thread::sleep_for(std::chrono::microseconds(20));
		if (m_commandListPool.size() <= 0)
		{
			CommandList* list = new CommandList();
			return list;
		}
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

	void GraphicsCommandContext::AddNewCommandList(CommandList* newList)
	{
		//CommandList* list = new CommandList();
		this->PushCommandList(newList);
	}
}






namespace AEngine::RenderCore
{
	GraphicsCommandAllocator* GraphicsCommandAllocator::m_uniqueObj;

	GraphicsCommandAllocator::GraphicsCommandAllocator()
	{

	}

	GraphicsCommandAllocator::~GraphicsCommandAllocator()
	{
		while (!m_commandAllocatorPool.empty())
		{
			var allocator = m_commandAllocatorPool.front();
			delete allocator;
			allocator = nullptr;
			m_commandAllocatorPool.pop();
		}
	}

	GraphicsCommandAllocator* GraphicsCommandAllocator::GetInstance()
	{
		if (m_uniqueObj == nullptr)
		{
			m_uniqueObj = new GraphicsCommandAllocator();
		}
		return m_uniqueObj;
	}

	CommandAllocator * GraphicsCommandAllocator::GetCommandAllocator()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		while (m_commandAllocatorPool.size() <= 0);
		lock_guard<mutex> lock(m_mutex);
		var allocator = m_commandAllocatorPool.front();
		m_commandAllocatorPool.pop();
		return allocator;
	}

	void GraphicsCommandAllocator::PushCommandAllocator(CommandAllocator * newAllocator)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		lock_guard<std::mutex> lock(m_mutex);
		m_commandAllocatorPool.emplace(newAllocator);
	}
}