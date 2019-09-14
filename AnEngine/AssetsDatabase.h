#pragma once
#ifndef __RESOURCEPOOL_H__
#define __RESOURCEPOOL_H__

#include "Mesh.h"
#include "Texture.h"
#include "onwind.h"
#include "Scene.h"

namespace AnEngine::AssetsWrapper
{
	class DLL_API AssetsDatabase : public Singleton<AssetsDatabase>
	{
		friend class Singleton<AssetsDatabase>;

		std::map<uint64_t, Resource::Mesh*> m_meshes;
		std::map<uint64_t, Resource::Texture*> m_textures;

		Game::Scene* m_editor;

	protected:
		friend class AnEngine::Resource::Mesh;
		friend class AnEngine::Resource::Texture;

		uint64_t AddMesh(AnEngine::Resource::Mesh* mesh);
		uint64_t AddTexture(AnEngine::Resource::Texture* tex);

	public:
		AssetsDatabase();
		~AssetsDatabase();

		void RefreshAssets();

		static const std::wstring s_AssetsPath;
	};

}

#endif // !__RESOURCEPOOL_H__
