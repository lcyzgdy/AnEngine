#include "GameObject.h"
#include "ObjectBehaviour.h"
using namespace std;

namespace AnEngine::Game
{
	//GameObject::GameObject(const std::wstring& _name) : gameObject(this), m_parentObject(nullptr), name(_name)
	GameObject::GameObject(const std::wstring& _name) : m_parentObject(nullptr), name(_name)
	{
	}

	GameObject::GameObject(std::wstring&& _name) : m_parentObject(nullptr), name(_name)
	{
	}

	GameObject::~GameObject()
	{
		for (var i : m_children)
		{
			delete i;
		}
		m_children.clear();
	}

	GameObject * GameObject::GetParent()
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
	}

	vector<ObjectBehaviour*> GameObject::GetComponents()
	{
		return m_component;
	}

	void GameObject::AddComponent(ObjectBehaviour* component)
	{
		lock_guard<mutex> lock(m_mutex);
		component->gameObject = this;
		//component->m_scene = this->m_scene;
		m_component.emplace_back(component);
	}

	void GameObject::RemoveComponent(ObjectBehaviour* component)
	{
		lock_guard<mutex> lock(m_mutex);
		for (var it = m_component.begin(); it != m_component.end(); ++it)
		{
			if (*it == component)
			{
				(*it)->OnRelease();
				m_component.erase(it);
				delete *it;
				break;
			}
		}
	}
}