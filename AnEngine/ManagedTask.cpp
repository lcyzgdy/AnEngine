#include "ManagedTask.hpp"
#include "onwind.h"
#include <map>
using namespace std;

namespace AnEngine::Utility
{
	map<uint64_t, SceneManagedTaskQueue*> u_map;

	SceneManagedTaskQueue::SceneManagedTaskQueue(uint64_t hash)
	{
		u_map[hash] = this;
	}

	SceneManagedTaskQueue::~SceneManagedTaskQueue()
	{
		InvokeAll();
	}

	void SceneManagedTaskQueue::Commit(ManagedTask&& task)
	{
		m_taskQueue.emplace(task);
	}

	void SceneManagedTaskQueue::InvokeAll()
	{
		while (!m_taskQueue.empty())
		{
			var task = move(m_taskQueue.front());
			task.Invoke();
			m_taskQueue.pop();
		}
	}

	SceneManagedTaskQueue * SceneManagedTaskQueue::GetSceneTask(uint64_t hash)
	{
		if (u_map.find(hash) == u_map.end()) throw exception("Task queue not found");
		return u_map[hash];
	}
}