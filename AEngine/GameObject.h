#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include"onwind.h"
#include"PhysicsCore.h"
using namespace AEngine::PhysicsCore;
using namespace std;

namespace AEngine
{
	class GameObject
	{
		vector<GameObject*> m_children;
		Transform m_transform;

	public:
		GameObject();
		~GameObject() = default;

		string name;
		GameObject* const gameObject;
		GameObject* parentObject;

		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnRender();
		virtual void OnRelease();

		template<typename T>
		T* GetChildByName(string& name)
		{
			for (auto i = 0; i < m_children.size(); i++)
			{
				if (m_children[i].name == name) return static_cast<T*>(gameObject);
			}
			return nullptr;
		}
	};
}
#endif // !__GAMEOBJECT_H__
