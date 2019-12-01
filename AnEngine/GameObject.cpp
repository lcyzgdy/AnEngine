#include "GameObject.h"
#include "ObjectBehaviour.h"
#include "SceneManager.h"
using namespace std;

namespace AnEngine::Game
{
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

	void GameObject::AddBehaviour(ObjectBehaviour* component)
	{
		component->gameObject = this;
		m_behaviour.emplace_back(component);
	}

	std::shared_ptr<GameObject> GameObject::Create(const std::string& name)
	{
		class MakeSharedHelper : public GameObject
		{
		public:
			explicit MakeSharedHelper(const std::string& _name) :GameObject(_name) {}
		};
		return make_shared<MakeSharedHelper>(name);
	}

	std::shared_ptr<GameObject> GameObject::Create(std::string&& name)
	{
		class MakeSharedHelper : public GameObject
		{
		public:
			explicit MakeSharedHelper(std::string&& _name) :GameObject(_name) {}
		};
		return make_shared<MakeSharedHelper>(move(name));
	}

	void GameObject::Destory(GameObject* gameObject)
	{
		if (gameObject->m_destoryed) throw exception("This object has already destoryed!");
		gameObject->m_destoryed = true;
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
