#include "ThreadPool.h"
using namespace std;

template<typename Thread>
ThreadPool<Thread>::ThreadPool() : m_stoped(false)
{

}

template<typename Thread>
ThreadPool<Thread>::ThreadPool(int size) : m_stoped(false)
{
	m_idleThreadNum = (size < 1) ? 1 : size;
	for (int i = 0; i < m_idleThreadNum; i++)
	{
		m_pool.emplace_back([this]()->
		{
			function<void()> task;
			{
				unique_lock<mutex> lock(this->m_mutex);
				this->m_cvTask.wait(lock, [this]()->
				{
					return this->m_stoped.load() || !this->m_tasks.empty();
				});
				if (this->m_stoped && this->m_tasks.empty())
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

template<typename Thread>
ThreadPool<Thread>::~ThreadPool()
{
	m_stoped.store(true);
	m_cvTask.notify_all();
	for (var th : m_pool)
	{
		th.detach();
	}
}

template<typename Thread>
ThreadPool<Thread> * ThreadPool<Thread>::GetInstance()
{
	static ThreadPool<Thread> threadPool;
	return &threadPool;
}

template<typename Thread>
int ThreadPool<Thread>::GetIdleThreadNum()
{
	return m_idleThreadNum;
}

template<typename Thread>
template<typename F, typename ...Args>
void ThreadPool<Thread>::Commit(F && f, Args && ...args)
{

}
