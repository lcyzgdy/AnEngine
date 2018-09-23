#include "FenceContext.h"
using namespace std;

namespace AnEngine::RenderCore
{
	tuple<Fence*> FenceContext::GetOne()
	{
		if (m_pool.empty())
		{
			var p = new Fence();
			return p;
		}
		lock_guard<mutex> lock(m_mutex);
		var p = m_pool.front();
		m_pool.pop();
		return { p };
	}

	void FenceContext::Push(Fence* fence)
	{
		lock_guard<mutex> lock(m_wait);
		m_waitQueue.push(fence);
	}

	void FenceContext::WaitAllFence()
	{
		lock_guard<mutex> lock(m_wait);
		while (!m_waitQueue.empty())
		{
			var fence = m_waitQueue.front();
			m_waitQueue.pop();
			if (fence->Complete())
			{
				m_pool.push(fence);
				continue;
			}
			m_waitQueue.push(fence);
		}
	}
}