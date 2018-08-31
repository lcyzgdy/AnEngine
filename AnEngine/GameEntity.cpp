#include "GameEntity.h"

using namespace std;

namespace AnEngine::Game
{
	/* -----------Static Mathod----------------------- */
	void GameEntity::SceneAddNewEntity(GameEntity* entity)
	{
		//uint64_t index = g_entities.size();
		g_entities.push_back(entity);
	}

	void GameEntity::SceneRemoveEntity(GameEntity* entity)
	{
		for (int i = 0; i < g_entities.size(); i++)
		{
			if (g_entities[i] == entity)
			{
				g_entities[i] = g_entities.back();
				g_entities.pop_back();
				return;
			}
		}
		throw exception("Entity not found");

		tuple<int, char, float> a;
	}
	/* ============Static Mathod=============== */





	GameEntity::GameEntity(const wstring& name) : Name(name)
	{
		SceneAddNewEntity(this);
	}

	GameEntity::GameEntity(wstring&& name) : Name(name)
	{
		SceneAddNewEntity(this);
	}

	GameEntity::~GameEntity()
	{
		SceneRemoveEntity(this);
	}

	void GameEntity::AddComponent(ComponentData* component)
	{
		m_components.push_back(component);
	}

	wstring GameEntity::ToString()
	{
		return Name;
	}
}