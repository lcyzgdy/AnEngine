#pragma once
#ifndef __RESOURCEPOOL_H__
#define __RESOURCEPOOL_H__

#include "Mesh.h"
#include "Texture.h"
#include "onwind.h"
#include "Scene.h"
#include "GameObject.h"

namespace AnEngine::AssetsWrapper
{
	class DLL_API AssetsDatabase : public Singleton<AssetsDatabase>
	{
		friend class Singleton<AssetsDatabase>;

		std::map<uint64_t, Resource::Mesh*> m_meshes;
		std::map<uint64_t, Resource::Texture*> m_textures;
		std::map<uint64_t, Game::GameObject*> m_objects;
		// Game::Scene* m_editor;

	public:
		AssetsDatabase();
		~AssetsDatabase();

		uint64_t AddMesh(AnEngine::Resource::Mesh* mesh);
		uint64_t AddTexture(AnEngine::Resource::Texture* tex);

		void RefreshAssets();

		static const std::string s_AssetsPath;
	};

}

#endif // !__RESOURCEPOOL_H__
