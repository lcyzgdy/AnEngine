#pragma once
#ifndef __THREAD_H__
#define __THREAD_H__

#include<thread>
#include<mutex>
#include<atomic>

class Thread
{
	std::thread m_thread;

public:
	Thread() = default;
};

#endif // !__THREAD_H__
