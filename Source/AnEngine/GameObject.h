#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "onwind.h"
#include "Object.h"
#include "Transform.h"
#include <mutex>
#include <queue>
#include <set>
#include "Archetype.h"
#include "3rdparty/MetatypeHash.hpp"


namespace AnEngine::AssetsWrapper
{
	class AssetsDatabase;
}

namespace AnEngine::Game
{
	class ObjectBehaviour;

	// 一个确定的物体
	class DLL_API GameObject : public Object
	{
		friend class Scene;
		friend class AssetsWrapper::AssetsDatabase;
		friend class std::shared_ptr<GameObject>;
		friend class std::unique_ptr<GameObject>;
		friend std::shared_ptr<GameObject> std::make_shared(std::string&&);

		template <class _Ty>
		friend void std::_Destroy_in_place<_Ty>(_Ty&) noexcept;
		template<typename _Ty, typename... _Types>
		friend void std::_Construct_in_place(_Ty& _Obj, _Types&&... _Args) noexcept(std::is_nothrow_constructible_v<_Ty, _Types...>);
		friend struct std::default_delete<GameObject>;

		std::mutex m_mutex;
		bool m_active;
		bool m_destoryed;
		uint64_t m_id; // 在Scene容器中的编号

		// Create的时候令destoryed为false，AddToScene的时候为m_id赋值，默认为-1。

	protected:
		// 当前物体的一些组件，比如渲染器、脚本等等。
		// Components of this object, e.g. script、renderer、rigidbody etc.
		std::map<double, ObjectBehaviour*> m_behaviour;
		// std::map<double, size_t> m_typeToId;
		std::map<double, ComponentBase*> m_component;



		std::string name;



	public:
		GameObject(const std::string& name);
		GameObject(std::string&& name);

		virtual ~GameObject();

		template<typename _Ty, typename = typename std::enable_if<std::is_base_of<ComponentBase, _Ty>::value, bool>::type>
		_Ty* GetComponent()
		{
			constexpr double typeCode = MetatypeHash::hash<_Ty>();
			if (m_component.find(typeCode) != m_component.end())
			{
				return (_Ty*)m_component[typeCode];
			}
			return nullptr;
		}

		template<typename _Ty, typename = typename std::enable_if<std::is_base_of<ComponentBase, _Ty>::value, bool>::type>
		_Ty* AddComponent()
		{
			constexpr double typeCode = MetatypeHash::hash<_Ty>();
			if (m_component.find(typeCode) != m_component.end())
			{
				return (_Ty*)m_component[typeCode];
			}
			var comp = new _Ty();
			comp->m_entity = this;
			m_component[typeCode] = comp;
			return comp;
		}

		template<typename T, typename = typename std::enable_if<std::is_base_of<ObjectBehaviour, T>::value, bool>::type>
		T* GetBehaviour()
		{
			if constexpr (std::is_base_of<ObjectBehaviour, T>::value == false)
			{
				throw std::exception("Type is not derived ObjectBehaviour");
			}
			constexpr double typeCode = MetatypeHash::hash<T>();
			if (m_behaviour.find(typeCode) != m_behaviour.end())
			{
				return m_behaviour[typeCode];
			}
			return nullptr;
		}

		template<typename T>
		T* AddBehaviour()
		{
			if constexpr (std::is_base_of<ObjectBehaviour, T>::value == false)
			{
				throw std::exception("Type is not derived ObjectBehaviour");
			}
			constexpr double typeCode = MetatypeHash::hash<T>();
			if (m_behaviour.find(typeCode) != m_behaviour.end())
			{
				return m_behaviour[typeCode];
			}

			var comp = new T();
			m_behaviour[typeCode] = comp;
			return comp;
		}

		inline bool Active() { if (m_destoryed) throw std::exception("This object has already destoryed!");	return m_active; }
		void Active(bool b);

		inline bool Destoryed() { return m_destoryed; }

		__forceinline uint32_t Id() { return m_id; }

	public:
		static std::shared_ptr<GameObject> Create(const std::string& name);
		static std::shared_ptr<GameObject> Create(std::string&& name);
		static void Destory(GameObject* gameObject);
		static GameObject* Find(const std::string& name);
		static GameObject* Find(std::string&& name);
	};
}
#endif // !__GAMEOBJECT_H__



/*
 * 最终还是在 GameObject 里面添加了Component和Behaviour，如果彻底抛弃Behaviour则一些本来很简单的功能实现起来十分麻烦，
 * 而且之前写好的状态机也会报废。
 */
