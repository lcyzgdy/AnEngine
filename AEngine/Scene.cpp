#include "Scene.h"
#include"ObjectBehaviour.h"

namespace AEngine::Game
{
	void Scene::OnInit()
	{
		for (var& i : m_objects)
		{
			i->OnInit();
		}
	}

	void Scene::BeforeUpdate()
	{
	}

	void Scene::OnUpdate()
	{
	}

	void Scene::AfterUpdate()
	{
	}

	void Scene::OnRelease()
	{
		for (var& i : m_objects)
		{
			RemoveObject(dynamic_cast<GameObject*>(dynamic_cast<ObjectBehaviour*>(i)));
		}
	}

	void Scene::AddObject(GameObject * obj)
	{
		for (var& i : obj->GetChildren())
		{
			AddObject(i);
		}

		var behaviour = dynamic_cast<ObjectBehaviour*>(obj);
		for (var& i : behaviour->GetComponents())
		{
			lock_guard<std::recursive_mutex> lock(m_recursiveMutex);
			if (i)
			{
				m_objects.emplace_back(dynamic_cast<BaseBehaviour*>(i));
			}
		}
		lock_guard<std::recursive_mutex> lock(m_recursiveMutex);
		m_objects.emplace_back(dynamic_cast<BaseBehaviour*>(behaviour));
	}

	void Scene::RemoveObject(GameObject * obj)
	{
		for (var& i : obj->GetChildren())
		{
			RemoveObject(i);
		}
		lock_guard<std::recursive_mutex> lock(m_recursiveMutex);
		var behaviour = dynamic_cast<BaseBehaviour*>(dynamic_cast<ObjectBehaviour*>(obj));
		behaviour->OnRelease();
	}
}