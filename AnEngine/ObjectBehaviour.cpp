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
			//Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::BeforeUpdate, this));
		}
	}

	void ObjectBehaviour::BeforeUpdate()
	{
		Debug::Log(gameObject->name + ToLPCWSTR(typeid(*this).name()) + L": BeforeUpdate");
		//{
			//lock_guard<mutex> lock(m_mutex);
		if (!m_active) return;
		//}
		//Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::OnUpdate, this));
	}

	void ObjectBehaviour::OnUpdate()
	{
		//Debug::Log(gameObject->name + ToLPCWSTR(typeid(*this).name()) + L": OnUpdate");
		//{
			//lock_guard<mutex> lock(m_mutex);
		if (!m_active) return;
		Update();
		//}
		//Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::AfterUpdate, this));
	}

	void ObjectBehaviour::AfterUpdate()
	{
		//Debug::Log(gameObject->name + ToLPCWSTR(typeid(*this).name()) + L": AfterUpdate");
		//{
			//lock_guard<mutex> lock(m_mutex);
		if (!m_active) return;
		LateUpdate();
		//}
		//Utility::ThreadPool::Commit(std::bind(&ObjectBehaviour::BeforeUpdate, this));
	}

	void ObjectBehaviour::OnRelease()
	{
		if (m_released) return;
		m_active = false;
		//lock_guard<mutex> lock(m_mutex);
		Destory();
		m_released = true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectBehaviour::Start()
	{
		Debug::Log(gameObject->name + ToLPCWSTR(typeid(*this).name()) + L": Start");
	}

	void ObjectBehaviour::OnActive() { }

	void ObjectBehaviour::Update() { }

	void ObjectBehaviour::LateUpdate() { }

	void ObjectBehaviour::OnInvalid() { }

	void ObjectBehaviour::Destory()
	{
		Debug::Log(gameObject->name + L": Destory");
	}

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