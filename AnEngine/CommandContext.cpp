#include "CommandContext.h"
#include "GraphicCard.h"
#include "RenderCore.h"

#define in :

namespace AnEngine::RenderCore
{
	//GraphicsCommandContext* GraphicsCommandContext::m_uniqueObj;

	GraphicsCommandContext::GraphicsCommandContext()
	{
		
	}

	GraphicsCommandContext::~GraphicsCommandContext()
	{
		//while (!m_commandListPool.empty())
		while (!m_pool.empty())
		{
			var list = m_pool.front();
			delete list;
			m_pool.pop();
		}
		for (var i in m_readyQueue)
		{
			delete i;
		}
		m_readyQueue.clear();
	}

	/*GraphicsCommandContext* GraphicsCommandContext::GetInstance()
	{
		if (m_uniqueObj == nullptr)
		{
			m_uniqueObj = new GraphicsCommandContext();
		}
		return m_uniqueObj;
	}*/

	CommandList* GraphicsCommandContext::GetOne()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		if (m_pool.size() <= 0)
		{
			CommandFormatDesc desc;
			desc.allocator = GraphicsCommandAllocator::GetInstance()->GetOne()->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc);
			return list;
		}
		//lock_guard<mutex> lock(m_mutex);
		var list = m_pool.front();
		m_pool.pop();
		return list;
	}

	void GraphicsCommandContext::Push(CommandList* list)
	{
		//lock_guard<mutex> lockw(m_writerMutex);
		//m_readyQueue.emplace_back(list);
		lock_guard<mutex> lockr(m_readerMutex);
		m_pool.emplace(list);
	}

	void GraphicsCommandContext::AddNew(CommandList* list)
	{
		lock_guard<mutex> lockr(m_readerMutex);
		m_pool.emplace(list);
	}

	/*vector<ID3D12CommandList*> GraphicsCommandContext::GetReady()
	{
		lock_guard<std::mutex> lock(m_writerMutex);
		vector<ID3D12CommandList*> temp;
		for (var i in m_readyQueue)
		{
			temp.emplace_back(i->GetCommandList());
		}
		return std::move(temp);
		//return temp;
	}*/

	void GraphicsCommandContext::PopulateFinished()
	{
		lock_guard<std::mutex> lock1(m_readerMutex);
		lock_guard<std::mutex> lock2(m_writerMutex);
		for (CommandList* i in m_readyQueue)
		{
			m_pool.emplace(i);
		}
		m_readyQueue.clear();
	}

	/*CommandList* GraphicsCommandContext::GetCommandList()
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
		m_readyQueue.emplace_back(list);
	}

	void GraphicsCommandContext::AddNewCommandList(CommandList* newList)
	{
		//CommandList* list = new CommandList();
		//this->PushCommandList(newList);
		lock_guard<mutex> lockr(m_readerMutex);
		m_commandListPool.emplace(newList);
	}

	vector<ID3D12CommandList*> GraphicsCommandContext::GetReadyCommandList()
	{
		lock_guard<std::mutex> lock(m_writerMutex);
		vector<ID3D12CommandList*> temp;
		for (CommandList* i : m_readyQueue)
		{
			temp.emplace_back(i->GetCommandList());
		}
		return std::move(temp);
		//return temp;
	}

	void GraphicsCommandContext::PopulateFinished()
	{
		lock_guard<std::mutex> lock1(m_readerMutex);
		lock_guard<std::mutex> lock2(m_writerMutex);
		for (CommandList* i : m_readyQueue)
		{
			m_commandListPool.emplace(i);
		}
		m_readyQueue.clear();
	}*/
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
		while (!m_pool.empty())
		{
			var allocator = m_pool.front();
			delete allocator;
			allocator = nullptr;
			m_pool.pop();
		}
		for (var i in m_readyQueue)
		{
			delete i;
		}
		m_readyQueue.clear();
	}

	GraphicsCommandAllocator* GraphicsCommandAllocator::GetInstance()
	{
		if (m_uniqueObj == nullptr)
		{
			m_uniqueObj = new GraphicsCommandAllocator();
		}
		return m_uniqueObj;
	}

	/*CommandAllocator* GraphicsCommandAllocator::GetCommandAllocator()
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
	}*/
	CommandAllocator* GraphicsCommandAllocator::GetOne()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		//while (m_commandAllocatorPool.size() <= 0);
		if (m_pool.size() <= 0)
		{
			var allocator = new CommandAllocator();
			return allocator;
		}
		lock_guard<mutex> lock(m_mutex);
		var allocator = m_pool.front();
		m_pool.pop();
		return allocator;
	}

	void GraphicsCommandAllocator::Push(CommandAllocator* allocator)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		lock_guard<std::mutex> lock(m_mutex);
		m_pool.emplace(allocator);
	}

	void GraphicsCommandAllocator::AddNew(CommandAllocator* allocator)
	{
	}

	void GraphicsCommandAllocator::PopulateFinished()
	{
	}
}