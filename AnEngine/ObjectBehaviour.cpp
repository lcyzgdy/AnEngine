#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"
#include "Scene.h"
#include "DebugLog.h"
using namespace AnEngine::Debug;

namespace AnEngine::Game
{
	void ObjectBehaviour::OnInit()
	{
		//lock_guard<mutex> lock(m_mutex);
		//if (m_scene == nullptr) m_scene = dynamic_cast<ObjectBehaviour*>(gameObject)->m_scene;
		Start();
		if (m_active) //BeginUpdate();
		{
			OnActive();
			//Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
		}
		/*for (var item : m_children)
		{
			dynamic_cast<ObjectBehaviour*>(item)->OnInit();
		}*/
		/*for (var item : m_component)
		{
			item->OnInit();
		}*/
	}

	void ObjectBehaviour::OnUpdate()
	{
		/*Debug::Log(name + L": Begin update");
		while (m_active)
		{
			//lock_guard<mutex> lock(m_mutex);
#if defined(_DEBUG) || defined(DEBUG)
			BeforeUpdate();
			//m_scene->Wait();
			Update();
			//m_scene->Wait();
			AfterUpdate();
			//m_scene->Wait();
			//Debug::Log(name + to_wstring(m_active));
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
		}*/
		if (!m_active) return;
		Update();
		/*for (var i : m_children)
		{
			dynamic_cast<ObjectBehaviour*>(i)->OnUpdate();
		}
		for (var i : m_component)
		{
			i->OnUpdate();
		}*/
	}

	void ObjectBehaviour::OnRelease()
	{
		if (m_released) return;
		m_active = false;
		lock_guard<mutex> lock(m_mutex);
		/*for (var i : m_component)
		{
			i->OnRelease();
			delete i;
		}*/
		Destory();
		m_released = true;
	}

	void ObjectBehaviour::BeforeUpdate()
	{
		if (!m_active) return;
	}

	void ObjectBehaviour::AfterUpdate()
	{
		if (!m_active) return;
		LateUpdate();
		/*for (var i : m_children)
		{
			dynamic_cast<ObjectBehaviour*>(i)->AfterUpdate();
		}
		for (var i : m_component)
		{
			i->AfterUpdate();
		}*/
	}

	void ObjectBehaviour::LateUpdate()
	{
	}

	void ObjectBehaviour::Start()
	{
		Debug::Log(gameObject->name + L": Start");
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
		Debug::Log(gameObject->name + L": Destory");
	}

	/*ObjectBehaviour::ObjectBehaviour(const std::wstring& name) : m_active(true)
	{
	}

	ObjectBehaviour::ObjectBehaviour(std::wstring&& name) : GameObject(name), m_active(true)
	{
	}*/

	ObjectBehaviour::ObjectBehaviour() : m_active(true)
	{
	}

	ObjectBehaviour::~ObjectBehaviour()
	{
		if (!m_released)
			OnRelease();
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