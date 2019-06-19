#include "FbxImporter.h"
#include "Mesh.h"

using namespace std;

namespace AnEngine::Resource
{
#ifdef OPENFBX
	std::byte* LoadFbxFromFile(const wstring& filePath)
	{
		ifstream fin(filePath, ios::binary);
		fin.seekg(0, ios::end);
		int fileLength = fin.tellg();
		fin.seekg(0, ios::beg);
		char* buffer = new char[fileLength];
		fin.read(buffer, fileLength);

		var scene = ofbx::load(reinterpret_cast<ofbx::u8*>(buffer), fileLength);

		vector<Mesh*> meshes;

		int meshCount = scene->getMeshCount();
		for (int i = 0; i < meshCount; i++)
		{
			var mesh = scene->getMesh(i);
			Mesh* tMesh = new Mesh();

			int vertexCount = mesh->getGeometry()->getVertexCount();
			for (int j = 0; j < vertexCount; j++)
			{
				var pos0 = mesh->getGeometry()->getVertices()[j];
				DMath::Vector4 pos(pos0.x, pos0.y, pos0.z, 0);
				tMesh->m_vertices.push_back(pos);
			}
			for (int j = 0; j < vertexCount; j++)
			{
				var nor0 = mesh->getGeometry()->getNormals()[j];
				DMath::Vector3 nor(nor0.x, nor0.y, nor0.z);
				tMesh->m_normals.push_back(nor);
			}
		}

		ofbx::Texture* tex;
		return nullptr;
	}
#endif // OPENFBX
}