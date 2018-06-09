#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "onwind.h"
#include "Object.h"
#include "Transform.h"
#include <mutex>
#include <queue>
// #include"BaseBehaviour.h"

namespace AnEngine::Game
{
	class ObjectBehaviour;

	// 一个确定的物体
	class GameObject : public Object
	{
		friend class Scene;

		std::mutex m_mutex;
		bool m_active;
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

		template<typename _Ty>
		std::vector<_Ty*>&& GetComponents()
		{
			std::vector<_Ty*> ret;

			return std::move(ret);
		}

		template<typename _Ty>
		std::vector<_Ty*> GetComponentsInChildren()
		{
			std::vector<_Ty*> ret;
			std::queue<GameObject*> q;
			for (var child : m_children)
			{
				q.push(child);
			}
			while (!q.empty())
			{
				for (var c : q.front()->m_component)
				{
					/*if (typeid(*c) == typeid(_Ty))
					{
						ret.push_back((_Ty*)c);
					}*/
					var p = dynamic_cast<_Ty*>(c);
					if (p != nullptr)
					{
						ret.push_back(p);
					}
				}
				for (var c : q.front()->m_children)
				{
					q.push(c);
				}
				q.pop();
			}

			return std::move(ret);
		}

		std::vector<ObjectBehaviour*> GetComponents();

		template<typename _Ty = ObjectBehaviour>
		_Ty* GetComponent()
		{
			for (var i : this->m_component)
			{
				if (typeid(*i) == typeid(_Ty))
				{
					return (_Ty*)i;
				}
				/*var p = dynamic_cast<_Ty*>(i);
				if (p != nullptr)
					return p;*/
			}
			return nullptr;
		}

		void AddComponent(ObjectBehaviour* component);

		template<typename _Ty>
		void AddComponent()
		{
			if (IsDerived<_Ty, ObjectBehaviour>::Result == false)
			{
				throw std::exception("_Ty is not derived ObjectBehaviour");
			}
			AddComponent(new _Ty());
		}

		void RemoveComponent(ObjectBehaviour* component);
		void AddChildObject(GameObject* obj);

		bool Active();
		void Active(bool b);

		static GameObject* Find(const std::wstring& name);
		static GameObject* Find(std::wstring&& name);
	};
}
#endif // !__GAMEOBJECT_H__
