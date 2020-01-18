#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include <condition_variable>
#include "onwind.h"
#include "BaseBehaviour.h"
#include "GameObject.h"
#include "ComponentGroup.hpp"
#include "SystemBase.h"
#include "Delegate.hpp"
#include "ChunkAllocator.h"

namespace AnEngine::Game
{
	// Scene仅作为GameObject的集合，同时待ECS完成后也是Entity或Component的集合。
	class DLL_API Scene : public Object//, public NonCopyable
	{
		std::queue<uint32_t> m_freeObjPos;			// 已经被删除掉的对象
		std::map<size_t, void*> m_componentGroups;
		std::deque<System::SystemBase*> m_systems;
		std::mutex m_behaviourMutex;

		// std::list<Archetype*> m_archetypeList;
		// std::map<GameObject*, Memory::Chunk*> m_objs;
		std::deque<GameObject*> m_objects;

	public:
		Scene(std::string&& _name);
		Scene(const std::string& _name);
		virtual ~Scene() = default;

		Delegate<void> onLoad;
		Delegate<void> onUnload;

		std::string name;

		void AddToScene(GameObject* obj);
		void RemoveObject(GameObject* obj);
		const std::deque<GameObject*>& GetAllGameObjects() { return m_objects; }

		template<typename _Ty>
		void AddComponent(const GameObject* const gameObject)
		{

		}

		template<typename T>
		void CreateSystem()
		{
			if constexpr (std::is_base_of<System::SystemBase, T>::value == false) throw std::exception("这不是一个System");
			for (var i : m_systems)
			{
				if (std::is_same<decltype(i), T>::value) throw std::exception("已经存在一个这种类型的System");
			}
			m_systems.emplace_back(new T());
		}

		const std::deque<System::SystemBase*>& GetAllSystems() { return m_systems; }
	};
}

#endif // !__SCENE_H__
