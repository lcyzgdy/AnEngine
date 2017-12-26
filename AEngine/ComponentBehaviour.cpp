#include "ComponentBehaviour.h"
#include "ThreadPool.hpp"

namespace AEngine::Game
{
	/*void ComponentBehaviour::OnInit()
	{
		lock_guard<mutex> lock(m_mutex);
	}

	void ComponentBehaviour::BeforeUpdate()
	{
	}*/

	void ComponentBehaviour::OnUpdate()
	{
		while (this->m_enable)
		{
			std::lock_guard<std::mutex> lock(this->m_recursiveMutex);
			this->BeforeUpdate();
			this->Update();
			this->AfterUpdate();
		}
	}

	/*void ComponentBehaviour::AfterUpdate()
	{
	}

	void ComponentBehaviour::OnRelease()
	{
		lock_guard<mutex> lock(m_mutex);
	}*/

	void ComponentBehaviour::OnEnable()
	{
		lock_guard<mutex> lock(m_recursiveMutex);
	}

	void ComponentBehaviour::OnDisable()
	{
		lock_guard<mutex> lock(m_recursiveMutex);
	}

	void ComponentBehaviour::Enable(bool enable)
	{
		m_enable = enable;
		if (enable)
		{
			OnEnable();
			Utility::u_s_threadPool.Commit(std::bind(&ComponentBehaviour::OnUpdate, this));
		}
		else
		{
			OnDisable();
		}
	}
}