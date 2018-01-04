#pragma once
#ifndef __COMMANDCONTEXT_H__
#define __COMMANDCONTEXT_H__

#include"DX.h"
#include<queue>
#include<mutex>
#include"onwind.h"
#include"CommandList.h"

namespace AEngine::RenderCore
{
	class GraphicsCard;

	// 渲染线程独占一个CommandList
	class GraphicsCommandContext : public NonCopyable
	{
		static GraphicsCommandContext* m_uniqueObj;

		queue<CommandList*> m_commandListPool;
		ComPtr<ID3D12Fence> m_fence;
		std::mutex m_readerMutex;
		std::mutex m_writerMutex;
		std::mutex m_mutex;

		ID3D12Device* m_device;

		GraphicsCommandContext();
		~GraphicsCommandContext();

	public:
		static GraphicsCommandContext* GetInstance();
		CommandList* GetCommandList();
		void PushCommandList(CommandList* list);

		void AddNewCommandList(CommandList* newList);
	};

	class GraphicsCommandAllocator : public NonCopyable
	{
		static GraphicsCommandAllocator* m_uniqueObj;

		queue<CommandAllocator*> m_commandAllocatorPool;
		ComPtr<ID3D12Fence> m_fence;
		std::mutex m_readerMutex;
		std::mutex m_writerMutex;
		std::mutex m_mutex;

		GraphicsCommandAllocator();
		~GraphicsCommandAllocator();

	public:
		//explicit GraphicsCommandAllocator(GraphicsCard* device, uint32_t n = 8);
		static GraphicsCommandAllocator* GetInstance();
		CommandAllocator* GetCommandAllocator();
		void PushCommandAllocator(CommandAllocator* newAllocator);
	};
}

#endif // !__COMMANDCONTEXT_H__
