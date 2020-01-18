#include "Scene.h"
#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"
#include "ManagedTask.hpp"
#include "Canvas.h"
#include "RenderCore.h"
#include "StateMachine.h"
#include "Camera.h"
#include "SceneManager.h"
using namespace std;
using namespace AnEngine::Utility;
using namespace AnEngine::RenderCore;

namespace AnEngine::Game
{
	Scene::Scene(string&& _name) : name(_name)
	{
		SceneManager::AddNewScene(this);
	}

	Scene::Scene(const string& _name) : name(_name)
	{
		SceneManager::AddNewScene(this);
	}

	void Scene::AddToScene(GameObject* obj)
	{
		obj->m_id = m_objects.size();
		obj->scene = this;
		m_objects.emplace_back(obj);

		// m_archetypeList.rbegin();
	}

	void Scene::RemoveObject(GameObject* obj)
	{

	}
}