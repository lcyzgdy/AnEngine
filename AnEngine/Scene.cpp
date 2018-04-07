#include "Scene.h"
#include"ObjectBehaviour.h"

namespace AnEngine::Game
{
	void Scene::OnInit()
	{
		for (var i : m_objects)
		{
			i->OnInit();
		}
	}

	/*void Scene::BeforeUpdate()
	{
	}*/

	void Scene::OnUpdate()
	{
	}

	/*void Scene::AfterUpdate()
	{
	}*/

	void Scene::OnRelease()
	{
		for (var i : m_objects)
		{
			RemoveObject(dynamic_cast<GameObject*>(dynamic_cast<ObjectBehaviour*>(i)));
		}
	}

	Scene::Scene(std::wstring _name) : name(_name)
	{
	}

	void Scene::AddObject(GameObject* obj)
	{
		for (var i : obj->GetChildren())
		{
			AddObject(i);
		}

		var behaviour = dynamic_cast<ObjectBehaviour*>(obj);
		behaviour->m_scene = this;
		for (var i : behaviour->GetComponents())
		{
			//lock_guard<std::recursive_mutex> lock(m_recursiveMutex);
			lock_guard<mutex> lock(m_mutex);
			if (i != nullptr && std::find(m_objects.begin(), m_objects.end(), i) == m_objects.end())
			{
				m_objects.emplace_back(dynamic_cast<BaseBehaviour*>(i));
			}
		}
		//lock_guard<std::recursive_mutex> lock(m_recursiveMutex);
		lock_guard<mutex> lock(m_mutex);
		m_objects.emplace_back(dynamic_cast<BaseBehaviour*>(behaviour));
	}

	void Scene::RemoveObject(GameObject* obj)
	{
		for (var i : obj->GetChildren())
		{
			RemoveObject(i);
		}
		var behaviour = dynamic_cast<ObjectBehaviour*>(obj);
		for (var i : behaviour->GetComponents())
		{
			RemoveObject(i);
		}
		//lock_guard<std::recursive_mutex> lock(m_recursiveMutex);
		lock_guard<mutex> lock(m_mutex);
		for (var it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if (*it == behaviour)
			{
				m_objects.erase(it);
				break;
			}
		}
		dynamic_cast<BaseBehaviour*>(behaviour)->OnRelease();
		behaviour = nullptr;
	}

	void Scene::Wait()
	{

		unique_lock<mutex> lock(m_mutex);
		m_complateCount++;
		if (m_complateCount == m_objects.size())
		{
			m_complateCount = 0;
			m_cv.notify_all();
			return;
		}
		lock.unlock();
		unique_lock<mutex> behaviourLock(m_behaviourMutex);
		m_cv.wait(behaviourLock);
	}
}