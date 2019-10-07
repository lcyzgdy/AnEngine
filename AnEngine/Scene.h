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
//#include "GameEntity.h"

namespace AnEngine::Game
{
	// Scene仅作为GameObject的集合，同时待ECS完成后也是Entity或Component的集合。
	class DLL_API Scene : public Object//, public NonCopyable
	{
		// Scene直接调度BaseBehaviour
		//friend class ::AnEngine::Engine;

		std::deque<GameObject*> m_objects;
		std::queue<uint32_t> m_freeObjPos;	// 已经被删除掉的对象

		std::map<size_t, void*> m_componentGroups;

		std::deque<System::SystemBase*> m_systems;


		//std::condition_variable m_cv;
		std::mutex m_behaviourMutex;
		//uint32_t m_complateCount;

	public:
		//Scene(std::wstring _name);
		Scene(std::string&& _name);
		Scene(const std::string& _name);
		virtual ~Scene() = default;

		// virtual void OnLoad() = 0;
		// virtual void OnUnload() = 0;
		Delegate<void> onLoad;
		Delegate<void> onUnload;

		std::string name;

		void AddObject(GameObject* obj);
		void RemoveObject(GameObject* obj);
		std::deque<GameObject*> GetAllGameObject() { return m_objects; }

		template<typename T>
		void CreateComponentGroup()
		{
			if (m_componentGroups.find(typeid(T).hash_code()) != m_componentGroups.end()) throw std::exception("已经有一个");
			var group = new ComponentGroup<T>();
			m_componentGroups[typeid(T).hash_code()] = group;
		}

		template<typename T>
		void CreateSystem()
		{
			if constexpr (std::is_base_of<System::SystemBase, T>::value == false) throw std::exception("这不是一个System");
			for (var i : m_systems)
			{
				if (std::is_same<decltype(i), T>::value) throw std::exception("已经存在一个这种类型的System");
				//if (typeid(decltype(i)).hash_code() != typeid(T).hash_code()) throw std::exception("已经存在一个这种类型的System");
			}
			m_systems.emplace_back(new T());
		}

		const std::deque<System::SystemBase*>& GetAllSystems() { return m_systems; }

		template<typename T>
		ComponentGroup<T>* GetGroupOfType()
		{
			return (ComponentGroup<T>*)m_componentGroups[typeid(T).hash_code()];
		}

		/*template<typename T>
		std::vector<T*> GetAllComponentOfType()
		{
			std::vector<T*> ret;
			for (var i : m_objects)
			{
				//if(i.)
			}
			return ret;
		}*/

		/*void AddEntity(ECBS::GameEntity* entity);
		void RemoveEntity(ECBS::GameEntity* entity);
		std::vector<ECBS::GameEntity*> GetAllEntities() { return m_entities; }*/
	};
}

#endif // !__SCENE_H__
