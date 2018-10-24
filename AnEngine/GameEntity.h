#pragma once
#ifndef __GAMEENTITY_H__
#define __GAMEENTITY_H__

#include "onwind.h"
#include "ComponentData.h"

namespace AnEngine::Game::ECBS
{
	class GameEntity : public Object
	{
		/* ------Static Mathod--------- */
		//static std::vector<GameEntity*> g_entities;
		//static std::unordered_multimap<uint64_t, uint64_t> g_strToEntity;
		//static void SceneAddNewEntity(GameEntity* entity);
		//static void GameEntity::SceneRemoveEntity(GameEntity* entity);
		/* ======Static Mathod========= */

		std::vector<ComponentData*> m_components;

	public:
		std::wstring Name;

	public:
		explicit GameEntity(const std::wstring& name);
		explicit GameEntity(std::wstring&& name);
		virtual ~GameEntity();

		void AddComponent(ComponentData* component);

		virtual std::wstring ToString() override;

		template<typename T>
		bool HasComponentOfType()
		{
			for (var i : m_components)
			{
				if (typeid(i) == typeid(T)) return true;
			}
			return false;
		}

		template<typename T0, typename... T>
		bool HasComponentOfType()
		{
			for (var i : m_components)
			{
				if (typeid(i) == typeid(T)) return HasComponentOfType<T...>();
			}
			return false;
		}

		/*template<typename T0, typename...T>
		static std::vector<GameEntity*> FindEntitiesByComponentType()
		{
			std::vector<GameEntity*> ret;
			for (int i = 0; i < g_entities.size(); i++)
			{
				var e = g_entities[i];
				if (e->HasComponentOfType<T0, T...>())
				{
					ret.push_back(e);
				}
			}
			return ret;
		}*/
	};
}

#endif // !__GAMEENTITY_H__
