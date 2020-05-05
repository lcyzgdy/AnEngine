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
		std::map<uint64_t, std::unique_ptr<Game::GameObject>> m_objects;

	public:
		AssetsDatabase();
		~AssetsDatabase();

		// uint64_t AddMesh(AnEngine::Resource::Mesh* mesh);
		// uint64_t AddTexture(AnEngine::Resource::Texture* tex);
		void RefreshAssets();

	public:
		Resource::Mesh* AllocMesh();

	public:
		static const std::string s_AssetsPath;
	};

}

#endif // !__RESOURCEPOOL_H__
