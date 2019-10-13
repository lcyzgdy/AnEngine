#include "AssetsDatabase.h"
#include "UniqueId.hpp"

using namespace std;
using namespace AnEngine::Resource;
using namespace AnEngine::Game;

namespace AnEngine::AssetsWrapper
{
	const string AssetsDatabase::s_AssetsPath = "";

	uint64_t AssetsDatabase::AddMesh(Mesh* mesh)
	{
		uint64_t instanceId = UniqueId::Instance()->GetUniqueId();
		m_meshes[instanceId] = mesh;
		return instanceId;
	}

	uint64_t AssetsDatabase::AddTexture(Texture* tex)
	{
		uint64_t instanceId = UniqueId::Instance()->GetUniqueId();
		m_textures[instanceId] = tex;
		return instanceId;
	}

	AssetsDatabase::AssetsDatabase()
	{
	}

	AssetsDatabase::~AssetsDatabase()
	{
		for (var i : m_meshes)
		{
			delete i.second;
		}
		for (var i : m_textures)
		{
			delete i.second;
		}
	}
	void AssetsDatabase::RefreshAssets()
	{
	}
}