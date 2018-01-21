#include "CommandContext.h"
#include "GraphicCard.h"
#include "RenderCore.h"

namespace AnEngine::RenderCore
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

	GraphicsCommandContext* GraphicsCommandContext::GetInstance()
	{
		if (m_uniqueObj == nullptr)
		{
			m_uniqueObj = new GraphicsCommandContext();
		}
		return m_uniqueObj;
	}

	CommandList* GraphicsCommandContext::GetCommandList()
	{
		//lock_guard<mutex> lockr(m_readerMutex);
		//while (m_commandListPool.size() <= 0) this_thread::sleep_for(std::chrono::microseconds(20));
		if (m_commandListPool.size() <= 0)
		{
			CommandFormatDesc desc;
			desc.allocator = GraphicsCommandAllocator::GetInstance()->GetCommandAllocator()->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc);
			return list;
		}
		//lock_guard<mutex> lock(m_mutex);
		lock_guard<mutex> lockr(m_readerMutex);
		var list = m_commandListPool.front();
		m_commandListPool.pop();
		return list;
	}

	void GraphicsCommandContext::PushCommandList(CommandList* list)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		//lock_guard<std::mutex> lock(m_mutex);
		//m_commandListPool.emplace(list);
		m_readyQueue.emplace(list);
	}

	void GraphicsCommandContext::AddNewCommandList(CommandList* newList)
	{
		//CommandList* list = new CommandList();
		//this->PushCommandList(newList);
		lock_guard<mutex> lockr(m_readerMutex);
		m_commandListPool.emplace(newList);
	}

	vector<ID3D12CommandList*>&& GraphicsCommandContext::GetReadyCommandList()
	{
		lock_guard<std::mutex> lock(m_writerMutex);
		vector<ID3D12CommandList*> temp;
		while (!m_readyQueue.empty())
		{
			temp.emplace_back(m_readyQueue.front()->GetCommandList());
			m_readyQueue.pop();
		}
		return std::move(temp);
	}
}






namespace AnEngine::RenderCore
{
	GraphicsCommandAllocator* GraphicsCommandAllocator::m_uniqueObj;

	GraphicsCommandAllocator::GraphicsCommandAllocator()
	{

	}

	/*GraphicsCommandAllocator::GraphicsCommandAllocator(GraphicsCard* device, uint32_t n)
	{
		for (int i = 0; i < n; i++)
		{
			CommandAllocator* allocator = new CommandAllocator(device->GetDevice());
			m_commandAllocatorPool.emplace(allocator);
		}
	}*/

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

	CommandAllocator* GraphicsCommandAllocator::GetCommandAllocator()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		//while (m_commandAllocatorPool.size() <= 0);
		if (m_commandAllocatorPool.size() <= 0)
		{
			var allocator = new CommandAllocator();
			return allocator;
		}
		lock_guard<mutex> lock(m_mutex);
		var allocator = m_commandAllocatorPool.front();
		m_commandAllocatorPool.pop();
		return allocator;
	}

	void GraphicsCommandAllocator::PushCommandAllocator(CommandAllocator* newAllocator)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		lock_guard<std::mutex> lock(m_mutex);
		m_commandAllocatorPool.emplace(newAllocator);
	}
}