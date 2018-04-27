#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include"onwind.h"
#include"Object.h"
#include"Transform.h"
#include<mutex>
// #include"BaseBehaviour.h"

namespace AnEngine::Game
{
	class ObjectBehaviour;

	// 一个确定的物体
	class GameObject : public Object
	{
		friend class Scene;

		std::mutex m_mutex;
	protected:
		// 当前物体的父物体
		GameObject * m_parentObject;

		// 当前物体的子物体
		std::vector<GameObject*> m_children;

		// 当前物体的一些组件，比如渲染器、脚本等等。
		// Components of this object, such script、renderer、rigidbody etc.
		std::vector<ObjectBehaviour*> m_component;

	public:
		explicit GameObject(const std::wstring& name);
		explicit GameObject(std::wstring&& name);
		virtual ~GameObject();

		std::wstring name;
		Transform transform;

		GameObject* GetParent();
		void SetParent(GameObject* newParent);

		std::vector<GameObject*> GetChildren();

		template<typename _Ty = GameObject>
		_Ty* GetChildByName(std::wstring& name)
		{
			//for (auto i = 0; i < m_children.size(); i++)
			for (var i : m_children)
			{
				if (i->name == name) return static_cast<_Ty*>(gameObject);
			}
			return nullptr;
		}

		std::vector<ObjectBehaviour*> GetComponents();

		/*template<typename T = ObjectBehaviour>
		T* GetComponentByName(std::wstring name)
		{
			for (var i : ((ObjectBehaviour*)gameObject)->m_component)
			{
				if (i->name == name)
					return i;
			}
		}*/

		template<typename _Ty = ObjectBehaviour>
		_Ty* GetComponent()
		{
			for (var i : this->m_component)
			{
				var p = dynamic_cast<_Ty*>(i);
				if (p != nullptr)
					return p;
			}
			return nullptr;
		}

		void AddComponent(ObjectBehaviour* component);
		void RemoveComponent(ObjectBehaviour* component);
	};
}
#endif // !__GAMEOBJECT_H__
