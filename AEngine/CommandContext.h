#pragma once
#ifndef __COMMANDCONTEXT_H__
#define __COMMANDCONTEXT_H__

#include"DX.h"
#include<queue>
#include<mutex>
#include"onwind.h"

namespace AEngine::RenderCore
{
	// 渲染线程独占一个CommandList
	class GraphicsCommandListPool : public NonCopyable
	{
		queue<ComPtr<ID3D12GraphicsCommandList>> m_cp_commandListPool;
		ComPtr<ID3D12Fence> m_fence;
		std::mutex m_mutex;

	public:
		GraphicsCommandListPool();
		~GraphicsCommandListPool() = default;

		static GraphicsCommandListPool* GetInstance();
		ID3D12GraphicsCommandList* GetCommandList();
		void PushCommandList(ID3D12GraphicsCommandList* list);
	};
}

#endif // !__COMMANDCONTEXT_H__
