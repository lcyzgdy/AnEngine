#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"

namespace AEngine::Game
{
	void ObjectBehaviour::OnInit()
	{
		Start();
		if (m_active) BeginUpdate();
	}

	void ObjectBehaviour::OnUpdate()
	{
		//Update();
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::AfterUpdate, this));
		lock_guard<mutex> lock(m_mutex);
		while (m_active)
		{
			BeforeUpdate();
			Update();
			AfterUpdate();
		}
	}

	void ObjectBehaviour::OnRelease()
	{
		lock_guard<mutex> lock(m_mutex);
		Destory();
	}

	void ObjectBehaviour::BeginUpdate()
	{
		if (!m_active) throw std::exception("Object is not active");
		//if (!m_enable) return;
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::BeforeUpdate, this));
		Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
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

	bool ObjectBehaviour::Active()
	{
		return m_active;
	}

	void ObjectBehaviour::Active(bool b)
	{
		m_active = b;
		if (b)
		{
			OnActive();
			BeginUpdate();
		}
		else
		{
			OnInvalid();
		}
	}
}