#include "GameObject.h"

namespace AEngine::Game
{
	void GameObject::DoNothing()
	{
	}
	GameObject::GameObject() :gameObject(this), m_parentObject(nullptr)
	{
	}

	GameObject * GameObject::GetParent()
	{
		return m_parentObject;
	}

	void GameObject::SetParent(GameObject * newParent)
	{
		m_parentObject = newParent;
	}
	std::vector<GameObject*> GameObject::GetChildren()
	{
		return std::vector<GameObject*>();
	}
}