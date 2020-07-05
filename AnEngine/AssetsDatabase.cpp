#include "AssetsDatabase.h"
#include "UniqueId.hpp"

#include "StringBuilder.hpp"


using namespace std;
using namespace AnEngine::Resource;
using namespace AnEngine::Game;

DLL_API DECLEAR_UNIQUE_OBJ(AnEngine::AssetsWrapper::AssetsDatabase);
DECLEAR_INSTANCE(AnEngine::AssetsWrapper::AssetsDatabase);
/*AnEngine::AssetsWrapper::AssetsDatabase* AnEngine::Utility::Singleton<AnEngine::AssetsWrapper::AssetsDatabase>::Instance()
{
	if (s_uniqueObj == nullptr)
	{
		s_uniqueObj = new AnEngine::AssetsWrapper::AssetsDatabase();
	}
	return s_uniqueObj;
}*/

namespace AnEngine::AssetsWrapper
{
	const string AssetsDatabase::s_AssetsPath = "";


	AssetsDatabase::AssetsDatabase()
	{
	}

	AssetsDatabase::~AssetsDatabase()
	{
		for (var&& i : m_meshes)
		{
			i.second.release();
		}
	}

	void AssetsDatabase::RefreshAssets()
	{
	}

	Mesh* AssetsDatabase::AllocMesh()
	{
		uint64_t instanceId = UniqueId::Instance()->GetUniqueId();
		m_meshes[instanceId] = make_unique<Mesh>();
		return m_meshes[instanceId].get();
	}

	GameObject* AssetsDatabase::AllocPrefab(const std::string& name)
	{
		uint64_t instanceId = UniqueId::Instance()->GetUniqueId();
		m_prefabs[instanceId] = unique_ptr<GameObject>(new GameObject(name));
		return m_prefabs[instanceId].get();
	}

	GameObject* AssetsDatabase::AllocPrefab(std::string&& name)
	{
		uint64_t instanceId = UniqueId::Instance()->GetUniqueId();
		m_prefabs[instanceId] = unique_ptr<GameObject>(new GameObject(std::move(name)));
		return m_prefabs[instanceId].get();
	}

	std::string AssetsDatabase::StatisticsMessage()
	{
		ThirdParty::StringBuilder<char> sb;
		sb.Append("Prefab count: ");
		sb.AppendLine(to_string(m_prefabs.size()));
		sb.Append("Mesh count: ");
		sb.AppendLine(to_string(m_meshes.size()));
		return sb.ToString();
	}
	std::ostream& AssetsDatabase::StatisticsMessageStream(std::ostream& ostream)
	{
		ostream << "Prefab count: " << m_prefabs.size() << endl;
		ostream << "Mesh count: " << m_meshes.size() << endl;
		return ostream;
	}
}