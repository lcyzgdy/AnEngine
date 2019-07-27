#include "ResourcePool.h"
#include "UniqueId.hpp"

using namespace std;

namespace AnEngine::Resource
{
	ResourcePool::ResourcePool()
	{
	}

	Mesh* ResourcePool::ImportMeshFromFile()
	{
		uint64_t newid = UniqueId::Instance()->GetUniqueId();
		m_meshes.emplace(newid, Mesh());
		return &m_meshes[newid];
	}

	Texture* ResourcePool::ImportTextureFromFile()
	{
		uint64_t newid = UniqueId::Instance()->GetUniqueId();
		m_textures.emplace(newid, Texture());
		return &m_textures[newid];
	}
}