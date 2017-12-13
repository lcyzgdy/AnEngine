#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include"onwind.h"
#include"PhysicsCore.h"
#include"BaseBehaviour.h"
using namespace AEngine::PhysicsCore;
using namespace std;

namespace AEngine::Game
{
	class GameObject
	{
	protected:
		vector<GameObject*> m_children;
		GameObject* m_parentObject;
		Transform m_transform;

	public:
		GameObject();
		~GameObject() = default;

		string name;
		GameObject* const gameObject;

		GameObject* GetParent();

		template<typename T = GameObject>
		T* GetChildByName(string& name)
		{
			//for (auto i = 0; i < m_children.size(); i++)
			for (var i : m_children)
			{
				if (i->name == name) return static_cast<T*>(gameObject);
			}
			return nullptr;
		}
	};
}
#endif // !__GAMEOBJECT_H__
