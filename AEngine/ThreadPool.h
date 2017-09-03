#pragma once
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include"onwind.h"
#include"Thread.h"

class ThreadPool : NonCopyable
{
private:
	using Task = std::function<void()>;

	int m_threadNum;
	mutex m_mutex;
	std::condition_variable m_condition;

	ThreadPool();
	Thread* CreateThread();

public:
	ThreadPool* GetInstance();
	Thread* GetThread();
	int GetThreadNum();
};

#endif // !__THREADPOOL_H__
