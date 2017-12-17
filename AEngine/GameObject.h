#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include"onwind.h"
#include"PhysicsCore.h"
#include"BaseBehaviour.h"
using namespace AEngine::PhysicsCore;

namespace AEngine::Game
{
	// 一个确定的物体
	class GameObject
	{
		virtual void DoNothing();
	protected:
		std::vector<GameObject*> m_children;
		// 当前物体的子物体
		GameObject* m_parentObject;
		Transform m_transform;

	public:
		GameObject();
		~GameObject() = default;

		string name;
		GameObject* const gameObject;

		GameObject* GetParent();
		void SetParent(GameObject* newParent);

		std::vector<GameObject*> GetChildren();

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
