#pragma once
#ifndef __COMMANDCONTEXT_H__
#define __COMMANDCONTEXT_H__

#include"DX.h"
#include<queue>
#include<mutex>
#include<tuple>
#include"onwind.h"
#include"CommandList.h"
#include"Fence.hpp"

namespace AnEngine::RenderCore
{
	class ContextTask
	{
		CommandList* m_list;
		CommandAllocator* m_allo;
		Fence* m_syncObject;
	public:
		ContextTask(CommandList* list, CommandAllocator* allocator, Fence* sync);
		ContextTask(std::tuple<CommandList*, CommandAllocator*, Fence*>&& tp);
		void Run();
	};
}

namespace AnEngine::RenderCore
{
	class GraphicsCard;

	template<typename _List, typename _Allocator>
	class CommandContext
	{
	protected:
		std::queue<_List> m_pool;
		std::queue<_Allocator> m_alloPool;

		std::mutex m_readerMutex;
		std::mutex m_writerMutex;
		std::mutex m_mutex;
	};
	/*/
	template<typename _List, typename _Allocator>
	class IContext
	{
	public:
		virtual std::tuple<_List, _Allocator> GetOne() = 0;
		virtual void Push(_List, _Allocator) = 0;
	};*/

	template<typename ... _T>
	class IContext
	{
	public:
		virtual std::tuple<_T...> GetOne() = 0;
		virtual void Push(_T...) = 0;
	};

	class GraphicsCommandContext : public ::Singleton<GraphicsCommandContext>, public IContext<CommandList*, CommandAllocator*>,
		public CommandContext<CommandList*, CommandAllocator*>
	{
		friend class ::Singleton<GraphicsCommandContext>;

		GraphicsCommandContext();
		~GraphicsCommandContext();

	public:
		// 通过 IContext 继承
		virtual std::tuple<CommandList*, CommandAllocator*> GetOne() override;
		virtual void Push(CommandList*, CommandAllocator*) override;
	};
}
namespace AnEngine::RenderCore::Private
{
	class ComputeCommandContext : public ::Singleton<ComputeCommandContext>, public IContext<CommandList*, CommandAllocator*>,
		public CommandContext<CommandList*, CommandAllocator*>
	{
	public:
		// 通过 IContext 继承
		virtual std::tuple<CommandList*, CommandAllocator*> GetOne() override;
		virtual void Push(CommandList*, CommandAllocator*) override;
	};
}
namespace AnEngine::RenderCore
{
	class FenceContext : public ::Singleton<FenceContext>, public IContext<Fence*>
	{
		std::queue<Fence*> m_pool;
	public:
		// 通过 IContext 继承
		virtual std::tuple<Fence*> GetOne() override;
		virtual void Push(Fence* fence) override;
	};
}

namespace AnEngine::RenderCore
{
	class ComputeContext
	{
	public:
		static std::tuple<CommandList*, CommandAllocator*> GetOne();
		static void Push(CommandList* list, CommandAllocator* allocator);
	};

	class GraphicsContext
	{
	public:
		static std::tuple<CommandList*, CommandAllocator*> GetOne();
		static void Push(CommandList* list, CommandAllocator* allocator);
	};
}


#endif // !__COMMANDCONTEXT_H__
