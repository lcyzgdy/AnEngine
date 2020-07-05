#pragma once
#ifndef __RESOURCEPOOL_H__
#define __RESOURCEPOOL_H__

#include "Mesh.h"
#include "onwind.h"
#include "Scene.h"
#include "GameObject.h"
#include "Singleton.h"

namespace AnEngine::AssetsWrapper
{
	class AssetsDatabase : public Utility::Singleton<AssetsDatabase>
	{
		friend class Utility::Singleton<AssetsDatabase>;

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

		DLL_API std::string StatisticsMessage();

		DLL_API std::ostream& StatisticsMessageStream(std::ostream& ostream);

		template<typename _Ty, typename = typename std::enable_if<std::is_base_of<AnEngine::Object, _Ty>::value, bool>::type>
		DLL_API _Ty* LoadAssetAtPath();

	public:
		static const std::string s_AssetsPath;
	};


	template<typename _Ty, typename>
	inline DLL_API _Ty* AssetsDatabase::LoadAssetAtPath()
	{
		return nullptr;
	}
}

#endif // !__RESOURCEPOOL_H__
