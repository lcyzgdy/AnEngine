#pragma once
#ifndef __RESOURCEPOOL_H__
#define __RESOURCEPOOL_H__

#include "Mesh.h"
#include "Texture.h"
#include "onwind.h"

namespace AnEngine::Resource
{
	class ResourcePool : Singleton<ResourcePool>
	{
		friend class Singleton<ResourcePool>;

		std::map<uint64_t, Mesh> m_meshes;
		std::map<uint64_t, Texture> m_textures;

	public:
		ResourcePool();
		Mesh* ImportMeshFromFile();
		Texture* ImportTextureFromFile();
	};
}

#endif // !__RESOURCEPOOL_H__
