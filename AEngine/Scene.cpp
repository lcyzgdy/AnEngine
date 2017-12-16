#include "Scene.h"
#include"ObjectBehaviour.h"

namespace AEngine::Game
{
	void Scene::OnInit()
	{
		for (var i : m_objects)
		{
			i->OnInit();
		}
	}

	void Scene::OnUpdate()
	{
	}

	void Scene::OnRelease()
	{
		for (let i : m_objects)
		{
			i->OnRelease();
		}
	}

	void Scene::AddObject(GameObject * obj)
	{
		for (var i : obj->GetChildren())
		{
			AddObject(i);
		}
		var behaviour = dynamic_cast<ObjectBehaviour*>(obj);
		m_objects.emplace_back(dynamic_cast<BaseBehaviour*>(behaviour));
	}
}