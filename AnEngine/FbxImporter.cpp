#include "FbxImporter.h"
#include "Mesh.h"
#include "ResourcePool.h"
#include "Vector.hpp"
#include "Color.h"

#if defined OPENFBX

#include "ofbx.h"

#elif defined ASSIMP

#include "assimp/scene.h"
#include "assimp/Importer.hpp"

#endif	

using namespace std;
using namespace AnEngine::DMath;
using namespace AnEngine::Resource;

namespace AnEngine::AssetsWrapper
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
#elif defined ASSIMP
	std::byte* LoadFbxFromFile(const wstring& filePath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("", 0);
		int meshCount = scene->mNumMeshes;
		if (meshCount > 0)
		{
			for (int i = 0; i < meshCount; i++)
			{
				Mesh* pMesh = AssetsDatabase::Instance()->ImportMeshFromFile();
				for (uint32_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					Vector3 vert(scene->mMeshes[i]->mVertices[j].x, scene->mMeshes[i]->mVertices[j].y, scene->mMeshes[i]->mVertices[j].z);
					pMesh->m_vertices.emplace_back(vert);

					if (scene->mMeshes[i]->HasVertexColors(j))
					{
						var pp = scene->mMeshes[i]->mColors[0];
						Color col(pp->r, pp->g, pp->b, pp->a);
						pMesh->m_colors.emplace_back(col);
					}
					if (scene->mMeshes[i]->HasTextureCoords(j))
					{
						// TODO:
					}
				}
				if (scene->mMeshes[i]->HasNormals())
				{
					for (uint32_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
					{
						Vector3 norl(scene->mMeshes[i]->mNormals[j].x, scene->mMeshes[i]->mNormals[j].y, scene->mMeshes[i]->mNormals[j].z);
						pMesh->m_normals.emplace_back(norl);
					}
				}
			}
		}
		return nullptr;
	}
#endif // OPENFBX
}