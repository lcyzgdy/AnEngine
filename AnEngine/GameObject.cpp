#include "GameObject.h"
#include "ObjectBehaviour.h"
#include "SceneManager.h"
using namespace std;

namespace AnEngine::Game
{
	//unordered_multimap<wstring, GameObject*> g_gameObjects;

	std::queue<GameObject*> GameObject::s_destoryed;

	//GameObject::GameObject(const std::wstring& _name) : gameObject(this), m_parentObject(nullptr), name(_name)
	GameObject::GameObject(const std::wstring& _name) : name(_name), m_active(true), m_id(-1)
	{
	}

	GameObject::GameObject(std::wstring&& _name) : name(_name), m_active(true), m_id(-1)
	{
	}

	GameObject::~GameObject()
	{
		/*for (var i : m_children)
		{
			delete i;
		}
		m_children.clear();*/
		/*var all = g_gameObjects.equal_range(this->name);
		while (all.first != all.second)
		{
			if ((*all.first).second == this)
			{
				g_gameObjects.erase(all.first);
				break;
			}
			++all.first;
		}*/
		for (var i : m_behaviour)
		{
			delete i;
		}
	}

	void GameObject::RealDestory()
	{
		while (!s_destoryed.empty())
		{
			delete s_destoryed.front();
			s_destoryed.pop();
		}
	}

	/*GameObject * GameObject::GetParent()
	{
		return m_parentObject;
	}

	void GameObject::SetParent(GameObject* newParent)
	{
		m_parentObject = newParent;
	}

	vector<GameObject*> GameObject::GetChildren()
	{
		return m_children;
	}*/

	void GameObject::AddBehaviour(ObjectBehaviour* component)
	{
		//lock_guard<mutex> lock(m_mutex);
		component->gameObject = this;
		//component->m_scene = this->m_scene;
		m_behaviour.emplace_back(component);
	}

	/*void GameObject::RemoveComponent(ObjectBehaviour* component)
	{
		lock_guard<mutex> lock(m_mutex);
		for (var it = m_behaviour.begin(); it != m_behaviour.end(); ++it)
		{
			if (*it == component)
			{
				(*it)->OnRelease();
				m_behaviour.erase(it);
				delete *it;
				break;
			}
		}
	}*/

	/*void GameObject::AddChildObject(GameObject* obj)
	{
		m_children.push_back(obj);
	}*/

	GameObject* GameObject::Create(const std::wstring& name)
	{
		if (s_destoryed.empty())
		{
			return new GameObject(name);
		}
		var ret = s_destoryed.front();
		s_destoryed.pop();
		return ret;
	}

	GameObject* GameObject::Create(std::wstring&& name)
	{
		if (s_destoryed.empty())
		{
			return new GameObject(name);
		}
		var ret = s_destoryed.front();
		ret->m_destoryed = false;
		s_destoryed.pop();
		return ret;
	}

	void GameObject::Destory(GameObject* gameObject)
	{
		if (gameObject->m_destoryed) throw exception("This object has already destoryed!");
		gameObject->m_destoryed = true;
		s_destoryed.push(gameObject);
	}

	GameObject* GameObject::Find(const std::wstring& name)
	{
		/*var r = g_gameObjects.find(name);
		if (r == g_gameObjects.end()) return nullptr;
		return (*r).second;*/
		var scene = SceneManager::ActiveScene();
		scene->GetAllGameObject();
		return nullptr;
	}

	GameObject* GameObject::Find(std::wstring&& name)
	{
		/*var r = g_gameObjects.find(name);
		if (r == g_gameObjects.end()) return nullptr;
		return (*r).second;*/
		return nullptr;
	}

	void GameObject::Active(bool b)
	{
		if (m_destoryed) throw exception("This object has already destoryed!");
		/*lock_guard<mutex> lock(m_mutex);*/
		m_active = b;
		if (b)
		{
			for (var i : m_behaviour)
			{
				i->OnActive();
			}
		}
		/*else
		{
			for (var i : GetComponents())
			{
				//i->OnInvalid();
			}
		}*/
		/*for (var i : m_children)
		{
			i->Active(b);
		}*/
	}
}
