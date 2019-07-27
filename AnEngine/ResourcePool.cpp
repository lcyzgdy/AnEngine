#include "ResourcePool.h"
#include "UniqueId.hpp"

namespace AnEngine::Resource
{
	ResourcePool::ResourcePool()
	{
	}

	Mesh* ResourcePool::ImportMeshFromFile()
	{
		uint64_t newid = UniqueId::Instance()->GetUniqueId();
		m_meshes[newid] = Mesh();
		return &m_meshes[newid];
	}

	Texture* ResourcePool::ImportTextureFromFile()
	{
	}
}