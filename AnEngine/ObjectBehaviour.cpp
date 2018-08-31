#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"
#include "Scene.h"
#include "DebugLog.h"
using namespace AnEngine::Debug;

namespace AnEngine::Game
{
	void ObjectBehaviour::OnInit()
	{
		Start();
		if (m_active)
		{
			OnActive();
		}
	}

	void ObjectBehaviour::BeforeUpdate()
	{
		if (!m_active) return;
	}

	void ObjectBehaviour::OnUpdate()
	{
		if (!m_active) return;
		Update();
	}

	void ObjectBehaviour::AfterUpdate()
	{
		if (!m_active) return;
		LateUpdate();
	}

	void ObjectBehaviour::OnRelease()
	{
		if (m_released) return;
		m_active = false;
		Destory();
		m_released = true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectBehaviour::Start() {	}

	void ObjectBehaviour::OnActive() { }

	void ObjectBehaviour::Update() { }

	void ObjectBehaviour::LateUpdate() { }

	void ObjectBehaviour::OnInvalid() { }

	void ObjectBehaviour::Destory() { }
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	ObjectBehaviour::ObjectBehaviour() : m_active(true)
	{
	}

	ObjectBehaviour::~ObjectBehaviour()
	{
		if (!m_released) OnRelease();
	}

	bool ObjectBehaviour::Active()
	{
		return m_active;
	}

	void ObjectBehaviour::Active(bool b)
	{
		lock_guard<mutex> lock(m_mutex);
		m_active = b;
		if (b)
		{
			OnActive();
			//Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
		}
		else
		{
			OnInvalid();
		}
	}
}