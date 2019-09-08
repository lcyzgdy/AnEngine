#pragma once
#ifndef __RESOURCEPOOL_H__
#define __RESOURCEPOOL_H__

#include "Mesh.h"
#include "Texture.h"
#include "onwind.h"

namespace AnEngine::AssetsWrapper
{
	class AssetsDatabase : public Singleton<AssetsDatabase>
	{
		friend class Singleton<AssetsDatabase>;

		std::map<uint64_t, Resource::Mesh> m_meshes;
		std::map<uint64_t, Resource::Texture> m_textures;

	public:
		AssetsDatabase();
		Resource::Mesh* ImportMeshFromFile();
		Resource::Texture* ImportTextureFromFile();

		static const std::wstring s_AssetsPath;
	};

}

#endif // !__RESOURCEPOOL_H__
