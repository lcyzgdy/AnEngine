#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"

namespace AEngine::Game
{
	void ObjectBehaviour::OnInit()
	{
		lock_guard<mutex> lock(m_recursiveMutex);
		Start();
		if (m_active) //BeginUpdate();
		{
			Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
		}
	}

	void ObjectBehaviour::OnUpdate()
	{
		//Update();
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::AfterUpdate, this));
		while (m_active)
		{
			lock_guard<mutex> lock(m_recursiveMutex);
			BeforeUpdate();
			Update();
			AfterUpdate();
		}
	}

	void ObjectBehaviour::OnRelease()
	{
		lock_guard<mutex> lock(m_recursiveMutex);
		for (var& i : m_component)
		{
			i->OnRelease();
		}
		Destory();
	}

	/*void ObjectBehaviour::BeginUpdate()
	{
		if (!m_active) throw std::exception("Object is not active");
		//if (!m_enable) return;
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::BeforeUpdate, this));
		Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
	}*/

	std::vector<ComponentBehaviour*> ObjectBehaviour::GetComponents()
	{
		return m_component;
	}

	void ObjectBehaviour::BeforeUpdate()
	{
		if (!m_active) return;
		//if (!m_enable) return;
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
	}

	void ObjectBehaviour::AfterUpdate()
	{
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::BeforeUpdate, this));
	}

	void ObjectBehaviour::AddComponent(ComponentBehaviour * component)
	{
		lock_guard<mutex> lock(m_recursiveMutex);
		m_component.emplace_back(component);
	}

	void ObjectBehaviour::RemoveComponent(ComponentBehaviour* component)
	{
		lock_guard<mutex> lock(m_recursiveMutex);
		for (var it = m_component.begin(); it != m_component.end(); ++it)
		{
			if (*it == component)
			{
				m_component.erase(it);
				break;
			}
		}
	}

	bool ObjectBehaviour::Active()
	{
		return m_active;
	}

	void ObjectBehaviour::Active(bool b)
	{
		lock_guard<mutex> lock(m_recursiveMutex);
		m_active = b;
		if (b)
		{
			OnActive();
			//BeginUpdate();
			Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
		}
		else
		{
			OnInvalid();
		}
	}
}