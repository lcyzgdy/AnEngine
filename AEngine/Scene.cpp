#include "Scene.h"

namespace AEngine::Game
{
	void Scene::OnInit()
	{
		for (var i : m_objects)
		{
			i->OnInit();
		}
	}

	void Scene::OnRunning()
	{
	}

	void Scene::OnRelease()
	{
	}
}