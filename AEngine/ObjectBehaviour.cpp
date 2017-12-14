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
		Update();
		Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::AfterUpdate, this));
	}

	void ObjectBehaviour::OnRelease()
	{
		Destory();
	}

	void ObjectBehaviour::BeginUpdate()
	{
		if (!m_active) throw std::exception("Object is not active");
		//if (!m_enable) return;
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::BeforeUpdate, this));
		Utility::u_s_threadPool.Commit([=]()->void
		{
			while (m_active)
			{
				BeforeUpdate();
				Update();
				AfterUpdate();
			}
		});
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