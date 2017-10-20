#include "GameObject.h"

namespace AEngine
{
	GameObject::GameObject() :gameObject(this), parentObject(nullptr)
	{
	}

	void GameObject::OnInit()
	{
	}

	void GameObject::OnUpdate()
	{

	}

	void GameObject::OnRender()
	{
	}

	void GameObject::OnRelease()
	{
	}
}