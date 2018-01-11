#include "ComponentBehaviour.h"
#include "ThreadPool.hpp"

namespace AnEngine::Game
{
	void ComponentBehaviour::OnInit()
	{
	}

	void ComponentBehaviour::BeforeUpdate()
	{
	}

	void ComponentBehaviour::OnUpdate()
	{
		while (this->m_enable)
		{
			std::lock_guard<std::recursive_mutex> lock(this->m_recursiveMutex);
			this->BeforeUpdate();
			this->Update();
			this->AfterUpdate();
		}
	}

	void ComponentBehaviour::Update()
	{
	}

	void ComponentBehaviour::AfterUpdate()
	{
	}

	void ComponentBehaviour::OnRelease()
	{
	}

	void ComponentBehaviour::OnEnable()
	{
		lock_guard<recursive_mutex> lock(m_recursiveMutex);
		Utility::u_s_threadPool.Commit(std::bind(&ComponentBehaviour::OnUpdate, this));
	}

	void ComponentBehaviour::OnDisable()
	{
		lock_guard<recursive_mutex> lock(m_recursiveMutex);
	}

	ComponentBehaviour::ComponentBehaviour(bool enable) :m_enable(enable)
	{
	}

	void ComponentBehaviour::Enable(bool enable)
	{
		m_enable = enable;
		if (enable)
		{
			OnEnable();
		}
		else
		{
			OnDisable();
		}
	}
}