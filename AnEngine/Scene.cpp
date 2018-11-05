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
	/*void Scene::OnInit()
	{
		queue<GameObject*> q;
		for (var i : m_objects)
		{
			q.push(i);
		}
		while (!q.empty())
		{
			var p = q.front();
			q.pop();
			for (var i : p->m_behaviour)
			{
				i->OnInit();
			}
			for (var i : p->m_children)
			{
				q.push(i);
			}
		}
		m_frameLoop = true;
	}

	void Scene::BeforeUpdate()
	{
		future<void> f1 = Utility::ThreadPool::Commit(StateMachine::StaticUpdate);
		f1.wait();
	}

	void Scene::OnUpdate()
	{
		queue<GameObject*> q;
		for (var item : m_objects)
		{
			q.push(item);
		}
		while (!q.empty())
		{
			var p = q.front();
			q.pop();
			for (var i : p->m_behaviour)
			{
				i->BeforeUpdate();
			}
			for (var i : p->m_children)
			{
				q.push(i);
			}
		}

		for (var item : m_objects)
		{
			q.push(item);
		}
		while (!q.empty())
		{
			var p = q.front();
			q.pop();
			for (var i : p->m_behaviour)
			{
				if (is_same<decltype(*i), StateMachine>::value)
				{
					continue;
				}
				if (i->Active())
				{
					i->OnUpdate();
				}
			}
			for (var i : p->m_children)
			{
				if (i->Active())
				{
					q.push(i);
				}
			}
		}

		for (var item : m_objects)
		{
			if (item->Active())
			{
				q.push(item);
			}
		}
		while (!q.empty())
		{
			var p = q.front();
			q.pop();
			for (var i : p->m_behaviour)
			{
				if (i->Active())
				{
					i->AfterUpdate();
				}
			}
			for (var i : p->m_children)
			{
				if (i->Active())
				{
					q.push(i);
				}
			}
		}
	}

	void Scene::AfterUpdate()
	{ }

	void Scene::OnRelease()
	{
		lock_guard<mutex> lock(m_mutex);
		m_frameLoop = false;
		for (var i : m_objects)
		{
			RemoveObject(i);
		}
	}*/

	Scene::Scene(wstring&& _name) : name(_name)
	{
		//m_objects.reserve(256);
		SceneManager::AddNewScene(this);
	}

	Scene::Scene(const wstring& _name) : name(_name)
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