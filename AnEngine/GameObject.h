#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "onwind.h"
#include "Object.h"
#include "Transform.h"
#include <mutex>
#include <queue>
#include "ComponentData.h"
#include <set>
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
		// 当前物体的一些组件，比如渲染器、脚本等等。
		// Components of this object, such script、renderer、rigidbody etc.
		std::vector<ObjectBehaviour*> m_behaviour;
		std::map<size_t, ComponentData*> m_component;

	public:
		explicit GameObject(const std::wstring& name);
		explicit GameObject(std::wstring&& name);
		virtual ~GameObject();

		std::wstring name;

		//GameObject* GetParent();
		//void SetParent(GameObject* newParent);

		//std::vector<GameObject*> GetChildren();

		/*template<typename _Ty = GameObject>
		_Ty* GetChildByName(std::wstring& name)
		{
			//for (auto i = 0; i < m_children.size(); i++)
			for (var i : m_children)
			{
				if (i->name == name) return static_cast<_Ty*>(gameObject);
			}
			return nullptr;
		}*/

		// 获取所有组件，至于组件里有啥以及这个函数有什么用以后再说
		std::vector<ComponentData*>&& GetComponents()
		{
			std::vector<ComponentData*> ret;

			for (var i : m_component)
			{
				ret.push_back(i.second);
			}

			return std::move(ret);
		}

		/*template<typename _Ty>
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
				for (var c : q.front()->m_behaviour)
				{
					/*if (typeid(*c) == typeid(_Ty))
					{
						ret.push_back((_Ty*)c);
					}
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
		}*/

		//std::vector<ObjectBehaviour*> GetComponents() { return m_behaviour; }

		template<typename _Ty>
		_Ty* GetComponent()
		{
			return m_component[typeid(_Ty).hash_code()];
		}

		void AddComponent(ObjectBehaviour* component);

		template<typename _Ty>
		void AddComponent()
		{
			if (IsDerived<_Ty, ObjectBehaviour>::Result == false)
			{
				throw std::exception("Type is not derived ObjectBehaviour");
			}
			if (GetComponent<_Ty>() == nullptr)	AddComponent(new _Ty());
		}

		//void RemoveComponent(ObjectBehaviour* component);
		//void AddChildObject(GameObject* obj);

		bool Active();
		void Active(bool b);

		static GameObject* Find(const std::wstring& name);
		static GameObject* Find(std::wstring&& name);
	};
}
#endif // !__GAMEOBJECT_H__



/* 最终还是在 GameObject 里面添加了Component和Behaviour，如果彻底抛弃Behaviour则一些本来很简单的功能实现起来十分麻烦，
 * 而且之前写好的状态机也会报废。
 */