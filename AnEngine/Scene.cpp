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
		//m_objects.reserve(256);
		SceneManager::AddNewScene(this);
	}

	Scene::Scene(const string& _name) : name(_name)
	{
		SceneManager::AddNewScene(this);
	}

	void Scene::AddObject(GameObject* obj)
	{
		obj->m_id = m_objects.size();
		m_objects.emplace_back(obj);
	}

	void Scene::RemoveObject(GameObject* obj)
	{
		/*for (var it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if (*it == obj)
			{
				m_objects.erase(it);
				break;
			}
		}
		delete obj;*/
	}

	/*void Scene::AddEntity(GameEntity* entity)
	{
		m_entities.push_back(entity);
	}

	void Scene::RemoveEntity(GameEntity* entity)
	{
		m_entities.erase(find(m_entities.begin(), m_entities.end(), entity));
	}*/
}