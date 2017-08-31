#include "ThreadPool.h"
#include<queue>

static queue<Thread*> s_threadQ;

Thread* ThreadPool::CreateThread()
{
	GetInstance()->m_threadNum++;
	Thread* t = new Thread();
	return t;
}

ThreadPool * ThreadPool::GetInstance()
{
	static ThreadPool threadPool;
	return &threadPool;
}

Thread * ThreadPool::GetThread()
{
	lock_guard<mutex> lock(m_mutex);
	if (!s_threadQ.empty())
	{
		var thread = s_threadQ.front();
		s_threadQ.pop();
		return thread;
	}
	else
	{
		return GetInstance()->CreateThread();
	}
}

int ThreadPool::GetThreadNum()
{
	return m_threadNum;
}
