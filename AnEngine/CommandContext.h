#pragma once
#ifndef __COMMANDCONTEXT_H__
#define __COMMANDCONTEXT_H__

#include"DX.h"
#include<queue>
#include<mutex>
#include<tuple>
#include"onwind.h"
#include"CommandList.h"

namespace AnEngine::RenderCore
{
	class GraphicsCard;

	template<typename T>
	class Context
	{
	protected:
		std::queue<T> m_pool;
		std::vector<T> m_readyQueue;

		std::mutex m_readerMutex;
		std::mutex m_writerMutex;
		std::mutex m_mutex;

	public:
		virtual T GetOne() = 0;
		virtual void Push(T) = 0;
		virtual void AddNew(T) = 0;
		virtual void PopulateFinished() = 0;
	};

	// 渲染线程独占一个CommandList
	class GraphicsCommandContext : public ::Singleton<GraphicsCommandContext>, public Context<CommandList*>
	{
		friend class ::Singleton<GraphicsCommandContext>;
		//static GraphicsCommandContext* m_uniqueObj;

		//queue<CommandList*> m_commandListPool;
		//vector<CommandList*> m_readyQueue;
		//ComPtr<ID3D12Fence> m_fence_cp;
		//std::mutex m_readerMutex;
		//std::mutex m_writerMutex;
		//std::mutex m_mutex;

		//ID3D12Device* m_device;

		GraphicsCommandContext();
		~GraphicsCommandContext();

	public:
		//static GraphicsCommandContext* GetInstance();
		//CommandList* GetCommandList();
		//void PushCommandList(CommandList* list);

		//void AddNewCommandList(CommandList* newList);


		// 通过 Context 继承
		virtual CommandList* GetOne() override;
		virtual void Push(CommandList*) override;
		virtual void AddNew(CommandList*) override;
		virtual void PopulateFinished() override;

		//vector<ID3D12CommandList*> GetReadyCommandList();
		//void PopulateFinished();
	};

	class GraphicsCommandAllocator : public NonCopyable, public Context<CommandAllocator*>
	{
		static GraphicsCommandAllocator* m_uniqueObj;

		//queue<CommandAllocator*> m_commandAllocatorPool;
		//ComPtr<ID3D12Fence> m_fence_cp;
		//std::mutex m_readerMutex;
		//std::mutex m_writerMutex;
		//std::mutex m_mutex;

		GraphicsCommandAllocator();
		~GraphicsCommandAllocator();

	public:
		//explicit GraphicsCommandAllocator(GraphicsCard* device, uint32_t n = 8);
		static GraphicsCommandAllocator* GetInstance();
		//CommandAllocator* GetCommandAllocator();
		//void PushCommandAllocator(CommandAllocator* newAllocator);

		// 通过 Context 继承
		virtual CommandAllocator* GetOne() override;
		virtual void Push(CommandAllocator*) override;
		virtual void AddNew(CommandAllocator*) override;
		virtual void PopulateFinished() override;
	};
}
namespace AnEngine::RenderCore::Private
{
	class ComputeCommandContext : public ::Singleton<ComputeCommandContext>, public Context<CommandList*>
	{
	public:
		// 通过 Context 继承
		virtual CommandList* GetOne() override;
		virtual void Push(CommandList* list) override;
		virtual void AddNew(CommandList* list) override;
		virtual void PopulateFinished() override;
	};

	class ComputeCommandAllocator : public ::Singleton<ComputeCommandAllocator>, public Context<CommandAllocator*>
	{
	public:
		// 通过 Context 继承
		virtual CommandAllocator* GetOne() override;
		virtual void Push(CommandAllocator* allocator) override;
		virtual void AddNew(CommandAllocator* allocator) override;
		virtual void PopulateFinished() override;
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
