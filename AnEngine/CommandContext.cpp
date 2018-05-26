#include "CommandContext.h"
#include "GraphicsCard.h"
#include "RenderCore.h"
#include "ManagedTask.hpp"
#include <functional>

#define in :

using namespace AnEngine::Utility;

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
}
/*
namespace AnEngine::RenderCore
{
	ContextTask::ContextTask(CommandList * list, CommandAllocator * allocator, Fence * sync) : m_list(list), m_allo(allocator), m_syncObject(sync)
	{
	}

	ContextTask::ContextTask(std::tuple<CommandList*, CommandAllocator*, Fence*>&& tp) : m_list(get<CommandList*>(tp)),
		m_allo(get<CommandAllocator*>(tp)), m_syncObject(get<Fence*>(tp))
	{
	}

	void ContextTask::Run()
	{
		ID3D12CommandList* lists[] = { m_list->GetCommandList() };
		r_graphicsCard[0]->ExecuteSync(_countof(lists), lists);
	}
}*/

namespace AnEngine::RenderCore::Private
{
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
namespace AnEngine::RenderCore
{
	std::tuple<Fence*> FenceContext::GetOne()
	{
		if (m_pool.empty())
		{
			var p = new Fence();
			return p;
		}
		var p = m_pool.front();
		m_pool.pop();
		return { p };
	}

	void FenceContext::Push(Fence* fence)
	{
		m_pool.push(fence);
	}
}

namespace AnEngine::RenderCore
{
	using namespace AnEngine::RenderCore::Private;

	tuple<CommandList*, CommandAllocator*> ComputeContext::GetOne()
	{
		return move(GraphicsCommandContext::GetInstance()->GetOne());
	}

	void ComputeContext::Push(CommandList* list, CommandAllocator* allocator)
	{
		ID3D12CommandList* ppcommandList[] = { list->GetCommandList() };
		r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		GraphicsCommandContext::GetInstance()->Push(list, allocator);
	}

	tuple<CommandList*, CommandAllocator*> GraphicsContext::GetOne()
	{
		return move(GraphicsCommandContext::GetInstance()->GetOne());
	}

	void GraphicsContext::Push(CommandList* list, CommandAllocator* allocator)
	{
		ID3D12CommandList* ppcommandList[] = { list->GetCommandList() };
		r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);

		var[fence] = FenceContext::GetInstance()->GetOne();
		var iFence = fence->GetFence();
		uint64_t fenceValue = fence->GetFenceValue();
		fenceValue++;
		r_graphicsCard[0]->GetCommandQueue()->Signal(iFence, fenceValue);
		fence->WaitForValue(fenceValue);

		GraphicsCommandContext::GetInstance()->Push(list, allocator);
		FenceContext::GetInstance()->Push(fence);
	}
}