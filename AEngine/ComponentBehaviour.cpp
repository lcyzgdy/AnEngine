#include "ComponentBehaviour.h"
#include "ThreadPool.hpp"

namespace AEngine::Game
{
	void ComponentBehaviour::OnInit()
	{
		lock_guard<mutex> lock(m_mutex);
	}

	void ComponentBehaviour::BeforeUpdate()
	{
	}

	void ComponentBehaviour::OnUpdate()
	{
	}

	void ComponentBehaviour::AfterUpdate()
	{
	}

	void ComponentBehaviour::OnRelease()
	{
		lock_guard<mutex> lock(m_mutex);
	}

	void ComponentBehaviour::OnEnable()
	{
		lock_guard<mutex> lock(m_mutex);
	}

	void ComponentBehaviour::OnDisable()
	{
		lock_guard<mutex> lock(m_mutex);
	}

	void ComponentBehaviour::Enable(bool enable)
	{
		m_enable = enable;
		if (enable)
		{
			OnEnable();
			Utility::u_s_threadPool.Commit([this]()
			{
				while (this->m_enable)
				{
					std::lock_guard<std::mutex> lock(this->m_mutex);
					this->BeforeUpdate();
					this->OnUpdate();
					this->AfterUpdate();
				}
			});
		}
		else
		{
			OnDisable();
		}
	}
}