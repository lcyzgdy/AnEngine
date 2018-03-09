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
		while (!m_alloPool.empty())
		{
			var list = m_alloPool.front();
			delete list;
			m_alloPool.pop();
		}
	}

	std::tuple<CommandList*, CommandAllocator*> GraphicsCommandContext::GetOne()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		if (m_pool.size() <= 0)
		{
			var allocator = new CommandAllocator();
			CommandFormatDesc desc;
			desc.allocator = allocator->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc);
			return { list ,allocator };
		}
		lock_guard<mutex> lock(m_mutex);
		var list = m_pool.front();
		var allocator = m_alloPool.front();
		m_pool.pop();
		m_alloPool.pop();
		return { list, allocator };
	}

	void GraphicsCommandContext::Push(CommandList* list, CommandAllocator* allo)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		lock_guard<mutex> lock(m_mutex);
		m_pool.push(list);
		m_alloPool.push(allo);
	}

	/*GraphicsCommandContext* GraphicsCommandContext::GetInstance()
	{
		if (m_uniqueObj == nullptr)
		{
			m_uniqueObj = new GraphicsCommandContext();
		}
		return m_uniqueObj;
	}*/

	/*CommandList* GraphicsCommandContext::GetOne()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		if (m_pool.size() <= 0)
		{
			var allocator = GraphicsCommandAllocator::GetInstance()->GetOne();
			CommandFormatDesc desc;
			desc.allocator = allocator->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc);
			GraphicsCommandAllocator::GetInstance()->Push(allocator);
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

	void GraphicsCommandContext::PopulateFinished()
	{
		lock_guard<std::mutex> lock1(m_readerMutex);
		lock_guard<std::mutex> lock2(m_writerMutex);
		for (CommandList* i in m_readyQueue)
		{
			m_pool.emplace(i);
		}
		m_readyQueue.clear();
	}*/
}
/*
namespace AnEngine::RenderCore
{
	GraphicsCommandAllocator* GraphicsCommandAllocator::m_uniqueObj;

	GraphicsCommandAllocator::GraphicsCommandAllocator()
	{

	}

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
*/
namespace AnEngine::RenderCore::Private
{
	/*CommandList* ComputeCommandContext::GetOne()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		if (m_pool.size() <= 0)
		{
			var allocator = ComputeCommandAllocator::GetInstance()->GetOne();
			CommandFormatDesc desc;
			desc.allocator = allocator->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			ComputeCommandAllocator::GetInstance()->Push(allocator);
			return list;
		}
		//lock_guard<mutex> lock(m_mutex);
		var list = m_pool.front();
		m_pool.pop();
		return list;
	}

	void ComputeCommandContext::Push(CommandList* list)
	{
		lock_guard<mutex> lockr(m_readerMutex);
		m_pool.emplace(list);
	}

	void ComputeCommandContext::AddNew(CommandList* list)
	{
		lock_guard<mutex> lockr(m_readerMutex);
		m_pool.emplace(list);
	}*/

	std::tuple<CommandList*, CommandAllocator*> ComputeCommandContext::GetOne()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		if (m_pool.size() <= 0)
		{
			var allocator = new CommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE);
			CommandFormatDesc desc;
			desc.allocator = allocator->GetAllocator();
			desc.nodeMask = 1;
			desc.pipelineState = nullptr;
			CommandList* list = new CommandList(desc, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			return { list ,allocator };
		}
		lock_guard<mutex> lock(m_mutex);
		var list = m_pool.front();
		var allocator = m_alloPool.front();
		m_pool.pop();
		m_alloPool.pop();
		return { list, allocator };
	}
	void ComputeCommandContext::Push(CommandList* list, CommandAllocator* allo)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		lock_guard<mutex> lock(m_mutex);
		m_pool.push(list);
		m_alloPool.push(allo);
	}
}
/*
namespace AnEngine::RenderCore::Private
{
	CommandAllocator* ComputeCommandAllocator::GetOne()
	{
		lock_guard<mutex> lockr(m_readerMutex);
		//while (m_commandAllocatorPool.size() <= 0);
		if (m_pool.size() <= 0)
		{
			var allocator = new CommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE);
			return allocator;
		}
		lock_guard<mutex> lock(m_mutex);
		var allocator = m_pool.front();
		m_pool.pop();
		return allocator;
	}

	void ComputeCommandAllocator::Push(CommandAllocator* allocator)
	{
		lock_guard<mutex> lockw(m_writerMutex);
		lock_guard<mutex> lock(m_mutex);
		m_pool.emplace(allocator);
	}

	void ComputeCommandAllocator::AddNew(CommandAllocator*)
	{
	}

	void ComputeCommandAllocator::PopulateFinished()
	{
	}
}
*/
namespace AnEngine::RenderCore
{
	using namespace AnEngine::RenderCore::Private;

	tuple<CommandList*, CommandAllocator*> ComputeContext::GetOne()
	{
		//var pList = ComputeCommandContext::GetInstance()->GetOne();
		//var pAllocator = ComputeCommandAllocator::GetInstance()->GetOne();
		//return { pList, pAllocator };
		return move(GraphicsCommandContext::GetInstance()->GetOne());
	}

	void ComputeContext::Push(CommandList* list, CommandAllocator* allocator)
	{
		//ComputeCommandContext::GetInstance()->Push(list);
		//ComputeCommandAllocator::GetInstance()->Push(allocator);
		GraphicsCommandContext::GetInstance()->Push(list, allocator);
	}

	tuple<CommandList*, CommandAllocator*> GraphicsContext::GetOne()
	{
		return move(GraphicsCommandContext::GetInstance()->GetOne());
	}

	void GraphicsContext::Push(CommandList* list, CommandAllocator* allocator)
	{
		GraphicsCommandContext::GetInstance()->Push(list, allocator);
	}
}