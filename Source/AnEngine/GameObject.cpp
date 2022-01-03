#include "GameObject.h"
#include "ObjectBehaviour.h"
#include "SceneManager.h"
using namespace std;

namespace AnEngine::Game
{
	GameObject::GameObject(const std::string& _name) : name(_name), m_active(true), m_id(-1)
	{
		AddComponent<Transform>();
	}

	GameObject::GameObject(std::string&& _name) : name(_name), m_active(true), m_id(-1)
	{
		AddComponent<Transform>();
	}

	GameObject::~GameObject()
	{
		for (var&& i : m_behaviour)
		{
			delete i.second;
		}
		for (var&& i : m_component)
		{
			delete i.second;
		}
	}

	std::shared_ptr<GameObject> GameObject::Create(std::string&& name)
	{
		return make_shared<GameObject>(name);
	}

	void GameObject::Destory(GameObject* gameObject)
	{
		if (gameObject->m_destoryed) throw exception("This object has already destoryed!");
		gameObject->m_destoryed = true;

		for (var&& child : gameObject->GetComponent<Transform>()->Children())
		{
			Destory(child->m_entity);
		}
	}

	GameObject* GameObject::Find(const std::string& name)
	{
		return nullptr;
	}

	GameObject* GameObject::Find(std::string&& name)
	{
		return nullptr;
	}

	void GameObject::Active(bool b)
	{
		if (m_destoryed) throw exception("This object has already destoryed!");
		/*lock_guard<mutex> lock(m_mutex);*/
		m_active = b;
		if (b)
		{
			for (var&& i : m_behaviour)
			{
				i.second->OnActive();
			}
		}
	}
}
