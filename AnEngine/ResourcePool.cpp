#include "AssetsDatabase.h"
#include "UniqueId.hpp"

using namespace std;
using namespace AnEngine::Resource;

namespace AnEngine::AssetsWrapper
{
	AssetsDatabase::AssetsDatabase()
	{
	}

	Mesh* AssetsDatabase::ImportMeshFromFile()
	{
		uint64_t newid = UniqueId::Instance()->GetUniqueId();
		m_meshes.emplace(newid, Mesh());
		return &m_meshes[newid];
	}

	Texture* AssetsDatabase::ImportTextureFromFile()
	{
		uint64_t newid = UniqueId::Instance()->GetUniqueId();
		m_textures.emplace(newid, Texture());
		return &m_textures[newid];
	}
}