#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"
#include "Scene.h"
#include "DebugLog.h"
using namespace AnEngine::Debug;

namespace AnEngine::Game
{
	void ObjectBehaviour::OnInit()
	{
		lock_guard<mutex> lock(m_mutex);
		if (m_scene == nullptr) m_scene = dynamic_cast<ObjectBehaviour*>(gameObject)->m_scene;
		Start();
		if (m_active) //BeginUpdate();
		{
			OnActive();
			Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
		}
	}

	void ObjectBehaviour::OnUpdate()
	{
		Debug::Log(name + L": Begin update");
		while (m_active)
		{
			lock_guard<mutex> lock(m_mutex);
#if defined(_DEBUG) || defined(DEBUG)
			BeforeUpdate();
			m_scene->Wait();
			Update();
			m_scene->Wait();
			AfterUpdate();
			m_scene->Wait();
			Debug::Log(name + to_wstring(m_active));
#else 			
			try
			{
				BeforeUpdate();
				Update();
				AfterUpdate();
			}
			catch (exception e)
			{
				Debug::Log(e.what());
		}
#endif
	}
}

	void ObjectBehaviour::OnRelease()
	{
		m_active = false;
		lock_guard<mutex> lock(m_mutex);
		for (var i : m_component)
		{
			i->OnRelease();
			delete i;
		}
		Destory();
	}

	std::vector<ObjectBehaviour*> ObjectBehaviour::GetComponents()
	{
		return m_component;
	}

	void ObjectBehaviour::BeforeUpdate()
	{
		if (!m_active) return;
	}

	void ObjectBehaviour::AfterUpdate()
	{
	}

	void ObjectBehaviour::Start()
	{
		Debug::Log(name + L": Start");
	}

	void ObjectBehaviour::OnActive()
	{
	}

	void ObjectBehaviour::Update()
	{
	}

	void ObjectBehaviour::OnInvalid()
	{
	}

	void ObjectBehaviour::Destory()
	{
		Debug::Log(name + L": Destory");
	}

	ObjectBehaviour::ObjectBehaviour(const std::wstring& name) : GameObject(name), m_active(true)
	{
	}

	ObjectBehaviour::ObjectBehaviour(std::wstring&& name) : GameObject(name), m_active(true)
	{
	}

	void ObjectBehaviour::AddComponent(ObjectBehaviour* component)
	{
		lock_guard<mutex> lock(m_mutex);
		component->gameObject = this;
		component->m_scene = this->m_scene;
		m_component.emplace_back(component);
	}

	void ObjectBehaviour::RemoveComponent(ObjectBehaviour* component)
	{
		m_scene->RemoveObject(component);
		lock_guard<mutex> lock(m_mutex);
		for (var it = m_component.begin(); it != m_component.end(); ++it)
		{
			if (*it == component)
			{
				(*it)->OnRelease();
				m_component.erase(it);
				delete *it;
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
		lock_guard<mutex> lock(m_mutex);
		m_active = b;
		if (b)
		{
			OnActive();
			Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
		}
		else
		{
			OnInvalid();
		}
	}
}