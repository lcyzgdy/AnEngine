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
	// 渲染线程独占一个CommandList
	class GraphicsCommandContext : public NonCopyable
	{
		queue<CommandList*> m_commandListPool;
		ComPtr<ID3D12Fence> m_fence;
		std::mutex m_readerMutex;
		std::mutex m_writerMutex;
		std::mutex m_mutex;

	public:
		GraphicsCommandContext();
		~GraphicsCommandContext() = default;

		static GraphicsCommandContext* GetInstance();
		CommandList* GetCommandList();
		void PushCommandList(CommandList* list);

		void AddNewCommandList();
	};
}

#endif // !__COMMANDCONTEXT_H__
