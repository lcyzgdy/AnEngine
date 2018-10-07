#include "CommandContext.h"
#include "GraphicsCard.h"
#include "RenderCore.h"
#include "ManagedTask.hpp"
#include "FenceContext.h"
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
	using namespace AnEngine::RenderCore::Private;

	tuple<CommandList*, CommandAllocator*> ComputeContext::GetOne()
	{
		return move(GraphicsCommandContext::Instance()->GetOne());
	}

	void ComputeContext::Push(CommandList* list, CommandAllocator* allocator)
	{
		ID3D12CommandList* ppcommandList[] = { list->GetCommandList() };
		r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		GraphicsCommandContext::Instance()->Push(list, allocator);
	}

	tuple<CommandList*, CommandAllocator*> GraphicsContext::GetOne()
	{
		return GraphicsCommandContext::Instance()->GetOne();
	}

	void GraphicsContext::Push(CommandList* list, CommandAllocator* allocator)
	{
		ID3D12CommandList* ppcommandList[] = { list->GetCommandList() };
		r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);

		var[fence] = FenceContext::Instance()->GetOne();
		var iFence = fence->GetFence();
		uint64_t fenceValue = fence->GetFenceValue();
		fenceValue++;
		r_graphicsCard[0]->GetCommandQueue()->Signal(iFence, fenceValue);
		//fence->WaitForValue(fenceValue);

		GraphicsCommandContext::Instance()->Push(list, allocator);
		FenceContext::Instance()->Push(fence);
	}
}