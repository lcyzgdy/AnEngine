#pragma once
#ifndef __FencePool_H__
#define __FencePool_H__

#include "onwind.h"
#include "IPool.h"
#include "Fence.h"
#include "Singleton.h"

namespace AnEngine::RenderCore
{
	// Fence用以维护不同设备间的同步，目前只考虑单GPU和CPU的同步
	class DLL_API FencePool : public Utility::Singleton<FencePool>, public IPool<Fence*>
	{
		friend class Utility::Singleton<FencePool>;

		std::queue<Fence*> m_pool;			// 储存可用的Fence对象
		std::queue<Fence*> m_waitQueue;		// 储存已经进入等待的对象
		std::mutex m_wait;
		std::mutex m_mutex;
	public:
		// 通过 IContext 继承
		virtual std::tuple<Fence*>&& GetOne() override;
		virtual void Push(Fence* fence) override;

		void WaitAllFence();				// 等待所有GPU事件完成，更新GPU有关数据前至少调用一次。为了方便可在Present后调用。
	};
}

#endif // !__FencePool_H__
