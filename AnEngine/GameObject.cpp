#include "GameObject.h"

namespace AnEngine::Game
{
	GameObject::GameObject(const std::wstring& _name) : gameObject(this), m_parentObject(nullptr), name(_name)
	{
	}

	GameObject::GameObject(std::wstring&& _name) : gameObject(this), m_parentObject(nullptr), name(_name)
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
}