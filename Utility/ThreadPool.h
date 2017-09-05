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

template<typename Thread>
class ThreadPool : NonCopyable
{
private:
	using Task = std::function<void()>;

	mutex m_mutex;
	vector<Thread> m_pool;
	queue<Task> m_tasks;
	std::condition_variable m_cvTask;

	atomic_int m_idleThreadNum;
	const int m_cnt_maxThreadNum = 8;
	atomic_bool m_stopped;

	ThreadPool();
	ThreadPool(int size);
	~ThreadPool();

public:
	ThreadPool* GetInstance();
	int GetIdleThreadNum();

	template<typename F, typename ... Args>
	var Commit(F&& f, Args&&... args);
};

#endif // !__THREADPOOL_H__

