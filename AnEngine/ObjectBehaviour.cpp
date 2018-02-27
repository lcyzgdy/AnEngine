#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"
#include "Scene.h"
#include "DebugLog.h"
using namespace AnEngine::Debug;

namespace AnEngine::Game
{
	void ObjectBehaviour::OnInit()
	{
		//lock_guard<recursive_mutex> lock(m_recursiveMutex);
		lock_guard<mutex> lock(m_mutex);
		Start();
		if (m_active) //BeginUpdate();
		{
			OnActive();
			Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
		}
	}

	void ObjectBehaviour::OnUpdate()
	{
		//Update();
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::AfterUpdate, this));
		while (m_active)
		{
			//lock_guard<recursive_mutex> lock(m_recursiveMutex);
			lock_guard<mutex> lock(m_mutex);
#if defined(_DEBUG) || defined(DEBUG)
			BeforeUpdate();
			Update();
			AfterUpdate();
#else 			
			try
			{
				BeforeUpdate();
				Update();
				AfterUpdate();
			}
			catch (exception e)
			{
				Debug::Debug::Log(e.what());
			}
#endif
		}
	}

	void ObjectBehaviour::OnRelease()
	{
		m_active = false;
		//lock_guard<recursive_mutex> lock(m_recursiveMutex);
		lock_guard<mutex> lock(m_mutex);
		for (var i : m_component)
		{
			i->OnRelease();
			delete i;
		}
		Destory();
		//m_scene->RemoveObject(this);
	}

	/*void ObjectBehaviour::BeginUpdate()
	{
		if (!m_active) throw std::exception("Object is not active");
		//if (!m_enable) return;
		//Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::BeforeUpdate, this));
		Utility::u_s_threadPool.Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
	}*/

	std::vector<ObjectBehaviour*> ObjectBehaviour::GetComponents()
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

	void ObjectBehaviour::Start()
	{
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
	}

	ObjectBehaviour::ObjectBehaviour(const std::wstring & name) : GameObject(name), m_active(true)
	{
	}

	ObjectBehaviour::ObjectBehaviour(std::wstring&& name) : GameObject(name), m_active(true)
	{
	}

	void ObjectBehaviour::AddComponent(ObjectBehaviour * component)
	{
		//lock_guard<recursive_mutex> lock(m_recursiveMutex);
		lock_guard<mutex> lock(m_mutex);
		component->gameObject = this;
		m_component.emplace_back(component);
	}

	void ObjectBehaviour::RemoveComponent(ObjectBehaviour* component)
	{
		m_scene->RemoveObject(component);
		//lock_guard<recursive_mutex> lock(m_recursiveMutex);
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
		//lock_guard<recursive_mutex> lock(m_recursiveMutex);
		lock_guard<mutex> lock(m_mutex);
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