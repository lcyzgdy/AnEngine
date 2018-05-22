#include "Scene.h"
#include "ObjectBehaviour.h"
#include "ThreadPool.hpp"
#include "ManagedTask.hpp"
using namespace std;
using namespace AnEngine::Utility;

namespace AnEngine::Game
{
	void Scene::OnInit()
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
			for (var i : p->m_component)
			{
				i->OnInit();
			}
			for (var i : p->m_children)
			{
				q.push(i);
			}
		}
		m_frameLoop = true;
		Utility::ThreadPool::Commit([this]
		{
			unique_lock<mutex> lock(this->m_mutex);
			lock.unlock();
			while (lock.lock(), m_frameLoop == true)
			{
				this->BeforeUpdate();
				this->OnUpdate();
				this->AfterUpdate();
				lock.unlock();
			}
		});
	}

	void Scene::BeforeUpdate()
	{
		SceneManagedTaskQueue::GetSceneTask(GetHashCode())->InvokeAll();
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
			for (var i : p->m_component)
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
			for (var i : p->m_component)
			{
				i->OnUpdate();
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
			for (var i : p->m_component)
			{
				i->AfterUpdate();
			}
			for (var i : p->m_children)
			{
				q.push(i);
			}
		}
	}

	void Scene::AfterUpdate()
	{
	}

	void Scene::OnRelease()
	{
		lock_guard<mutex> lock(m_mutex);
		m_frameLoop = false;
		for (var i : m_objects)
		{
			RemoveObject(i);
		}
	}

	Scene::Scene(std::wstring _name) : name(_name)
	{
		SceneManagedTaskQueue* queue = new SceneManagedTaskQueue(GetHashCode());
	}

	void Scene::AddObject(GameObject* obj)
	{
		m_objects.emplace_back(obj);
	}

	void Scene::RemoveObject(GameObject* obj)
	{
		lock_guard<mutex> lock(m_mutex);
		for (var it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if (*it == obj)
			{
				m_objects.erase(it);
				break;
			}
		}
		delete obj;
	}

	void Scene::Wait()
	{
		{
			lock_guard<mutex> lock(m_mutex);
			m_complateCount++;
			if (m_complateCount == m_objects.size())
			{
				m_complateCount = 0;
				m_cv.notify_all();
				return;
			}
		}
		unique_lock<mutex> behaviourLock(m_behaviourMutex);
		m_cv.wait(behaviourLock);
	}
}