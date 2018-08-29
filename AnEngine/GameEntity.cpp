#include "GameEntity.h"

using namespace std;

namespace AnEngine::Game
{
	vector<GameEntity*> g_entities;
	unordered_multimap<uint64_t, uint64_t> g_strToEntity;

	void SceneAddNewEntity(GameEntity* entity)
	{
		//uint64_t index = g_entities.size();
		g_entities.push_back(entity);
	}

	void SceneRemoveEntity(GameEntity* entity)
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

	void GameEntity::AddComponent(ComponentBase* component)
	{
		m_components.push_back(component);
	}

	wstring GameEntity::ToString()
	{
		return Name;
	}
}