#pragma once
#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

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

namespace AnEngine::Utility::ThreadPool
{
	namespace Private
	{
		class ThreadPool : NonCopyable
		{
		private:
			using Task = std::function<void()>;

			std::mutex m_mutex;
			std::vector<std::thread> m_pool;
			std::queue<Task> m_tasks;
			std::condition_variable m_cvTask;

			std::atomic_int m_idleThreadNum;
			const uint32_t m_cnt_maxThreadNum = std::thread::hardware_concurrency();
			std::atomic_bool m_stopped;

		public:
			ThreadPool() = delete;

			ThreadPool(int size)
			{
				m_idleThreadNum = (size < 1) ? 1 : size;
				for (int i = 0; i < m_idleThreadNum; i++)
				{
					m_pool.emplace_back([this]()
					{
						while (!this->m_stopped)
						{
							std::function<void()> task;
							{
								std::unique_lock<std::mutex> lock(this->m_mutex);
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
						}
					});
				}
			}

			~ThreadPool()
			{
				m_stopped.store(true);
				m_cvTask.notify_all();
				for (size_t i = 0; i < m_pool.size(); i++)
				{
					m_pool[i].join();
				}
			}

			int GetIdleThreadNum()
			{
				return m_idleThreadNum;
			}

			template<typename F, typename ... Args>
			var Commit(F&& f, Args&& ...args)// -> std::future<decltype(f(args...))>
			{
				if (m_stopped.load())
				{
					throw exception("Thread pool is stopped");
				}
				using FuncType = decltype(f(args ...));
				var task = std::make_shared<std::packaged_task<FuncType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
				var awaitFuture = task->get_future();
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					m_tasks.emplace([task] { (*task)(); });
				}
				m_cvTask.notify_one();
				return std::move(awaitFuture);
			}
		};

		extern ThreadPool u_s_threadPool;
	}

	template<typename F, typename ... Args>
	var Commit(F && f, Args && ...args)
	{
		return Private::u_s_threadPool.Commit(f, args...);
	}

	template<typename F, typename ... Args>
	var CommitAfter(std::future<void>&& wait, F&& f, Args&& ... args)
	{
		wait.get();
		return Private::u_s_threadPool.Commit(f, args...);
	}
}
#endif // !__THREADPOOL_H__

