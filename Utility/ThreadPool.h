#pragma once
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include"onwind.h"
#include<thread>
#include<mutex>
#include<atomic>
#include<vector>
#include<queue>
#include<functional>
#include<condition_variable>
#include<stdexcept>
#include<future>
using namespace std;

class ThreadPool : NonCopyable
{
private:
	using Task = function<void()>;

	mutex m_mutex;
	vector<thread> m_pool;
	queue<Task> m_tasks;
	condition_variable m_cvTask;

	atomic_int m_idleThreadNum;
	const uint32_t m_cnt_maxThreadNum = thread::hardware_concurrency();
	atomic_bool m_stopped;


public:
	ThreadPool() = default;

	ThreadPool(int size)
	{
		m_idleThreadNum = (size < 1) ? 1 : size;
		for (int i = 0; i < m_idleThreadNum; i++)
		{
			m_pool.emplace_back([this]()
			{
				function<void()> task;
				{
					unique_lock<mutex> lock(this->m_mutex);
					this->m_cvTask.wait(lock, [this]()
					{
						return this->m_stopped.load() || !this->m_tasks.empty();
					});
					if (this->m_stopped && this->m_tasks.empty())
					{
						return;
					}
					task = move(this->m_tasks.front());
					this->m_tasks.pop();
				}
				m_idleThreadNum--;
				task();
				m_idleThreadNum++;
			});
		}
	}

	~ThreadPool()
	{
		m_stopped.store(true);
		m_cvTask.notify_all();
		for (size_t i = 0; i < m_pool.size(); i++)
		{
			m_pool[i].detach();
		}
	}

	int GetIdleThreadNum()
	{
		return m_idleThreadNum;
	}

	template<typename F, typename ... Args>
	var Commit(F && f, Args && ...args)
	{
		if (m_stopped.load())
		{
			throw exception("Thread pool is stopped");
		}
		using FuncType = decltype(f(args ...));
		var task = std::make_shared<packaged_task<FuncType()>>(bind(forward<F>(f), forward<Args>(args)...));
		future<FuncType> awaitFuture = task->get_future();
		{
			std::lock_guard<mutex> lock(m_mutex);
			m_tasks.emplace([task]()->{ (*task)(); });
		}
		m_cvTask.notify_one();
		return awaitFuture;
	}
};

#endif // !__THREADPOOL_H__

