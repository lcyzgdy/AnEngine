#include "GameObject.h"
#include "ObjectBehaviour.h"
#include "SceneManager.h"
using namespace std;

namespace AnEngine::Game
{
	std::queue<GameObject*> GameObject::s_destoryed;

	//GameObject::GameObject(const std::wstring& _name) : gameObject(this), m_parentObject(nullptr), name(_name)
	GameObject::GameObject(const std::string& _name) : name(_name), m_active(true), m_id(-1)
	{
	}

	GameObject::GameObject(std::string&& _name) : name(_name), m_active(true), m_id(-1)
	{
	}

	GameObject::~GameObject()
	{
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

	void GameObject::AddBehaviour(ObjectBehaviour* component)
	{
		//lock_guard<mutex> lock(m_mutex);
		component->gameObject = this;
		//component->m_scene = this->m_scene;
		m_behaviour.emplace_back(component);
	}

	GameObject* GameObject::Create(const std::string& name)
	{
		if (s_destoryed.empty())
		{
			return new GameObject(name);
		}
		var ret = s_destoryed.front();
		s_destoryed.pop();
		return ret;
	}

	GameObject* GameObject::Create(std::string&& name)
	{
		if (s_destoryed.empty())
		{
			var ret = new GameObject(name);
			return ret;
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

	GameObject* GameObject::Find(const std::string& name)
	{
		/*var r = g_gameObjects.find(name);
		if (r == g_gameObjects.end()) return nullptr;
		return (*r).second;*/
		var scene = SceneManager::ActiveScene();
		scene->GetAllGameObjects();
		return nullptr;
	}

	GameObject* GameObject::Find(std::string&& name)
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
	}
}
