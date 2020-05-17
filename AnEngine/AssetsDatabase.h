#pragma once
#ifndef __RESOURCEPOOL_H__
#define __RESOURCEPOOL_H__

#include "Mesh.h"
#include "onwind.h"
#include "Scene.h"
#include "GameObject.h"

namespace AnEngine::AssetsWrapper
{
	class DLL_API AssetsDatabase : public Singleton<AssetsDatabase>
	{
		friend class Singleton<AssetsDatabase>;

		std::map<uint64_t, std::unique_ptr<Resource::Mesh>> m_meshes;
		std::map<uint64_t, std::unique_ptr<Game::GameObject>> m_prefabs;

	public:
		AssetsDatabase();
		~AssetsDatabase();


		void RefreshAssets();

	public:
		Resource::Mesh* AllocMesh();
		Game::GameObject* AllocPrefab(const std::string& name);
		Game::GameObject* AllocPrefab(std::string&& name);

		std::string StatisticsMessage();
		std::ostream& StatisticsMessageStream(std::ostream& ostream);

	public:
		static const std::string s_AssetsPath;
	};

}

#endif // !__RESOURCEPOOL_H__
