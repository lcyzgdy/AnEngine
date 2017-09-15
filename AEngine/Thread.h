#pragma once
#ifndef __THREAD_H__
#define __THREAD_H__

#include<thread>
#include<mutex>
#include<atomic>

class RenderThread : std::thread
{
	
public:
	RenderThread() = default;
};

#endif // !__THREAD_H__
