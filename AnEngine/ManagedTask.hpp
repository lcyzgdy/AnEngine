#pragma once
#ifndef __MANAGEDTASK_HPP__
#define __MANAGEDTASK_HPP__

#include<functional>
#include<queue>
#include"Object.h"

namespace AnEngine::Utility
{
	//template<typename Func, typename ... Args>
	class ManagedTask : public Object
	{
		//std::function<Func(Args ...)> m_task;
		std::function<void()> m_task;

	public:
		ManagedTask()
		{
		}

		auto Invoke()
		{
			return m_task();
		}
	};

	class SceneManagedTaskQueue : public Object
	{
		std::queue<ManagedTask> m_taskQueue;

	public:
		SceneManagedTaskQueue(uint64_t hash);
		~SceneManagedTaskQueue();
		void Commit(ManagedTask&& task);
		void InvokeAll();

		static SceneManagedTaskQueue* GetSceneTask(uint64_t);
	};
}

#endif // !__MANAGEDTASK_HPP__
