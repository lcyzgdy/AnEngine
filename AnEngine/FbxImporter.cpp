#include "FbxImporter.h"
#include "Mesh.h"
#include "Vector.hpp"
#include "Color.h"
#include "AssetsDatabase.h"
#include "GameObject.h"
#include "Transform.h"

#include <filesystem>

#if defined OPENFBX

#include "ofbx.h"

#elif defined ASSIMP

#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#endif	

using namespace std;
using namespace AnEngine::DMath;
using namespace AnEngine::Resource;
using namespace AnEngine::Game;

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

	tuple<Vector3, Quaternion, Vector3> AssimpMatrixToTRS(const aiMatrix4x4& mat)
	{
		aiVector3D pos;
		aiQuaternion rot;
		aiVector3D scl;
		mat.Decompose(scl, rot, scl);
		return make_tuple<Vector3, Quaternion, Vector3>(Vector3(pos.x, pos.y, pos.z),
			Quaternion(rot.x, rot.y, rot.z, rot.w), Vector3(scl.x, scl.y, scl.z));
	}

	GameObject* BuildGameObjectRecurate(aiNode* root)
	{
		std::cout << root->mName.C_Str() << ' ' << root->mNumMeshes << std::endl;

		var go = new GameObject(root->mName.C_Str());
		Transform* trans = go->GetComponent<Transform>();
		var&& [pos, rot, scl] = AssimpMatrixToTRS(root->mTransformation);
		trans->LocalPosition(pos);
		trans->LocalRotation(rot);
		trans->LocalScale(scl);

		if (root->mNumMeshes > 1)
		{

		}

		for (int i = 0; i < root->mNumChildren; i++)
		{
			GameObject* child = BuildGameObjectRecurate(root->mChildren[i]);
			if (child == nullptr)
			{
				continue;
			}
			trans->AddChild(child->GetComponent<Transform>());
		}
		return go;
	}

	inline LoadAssetsStatusCode LoadFbxFromFile_Internal(string&& filePath)
	{
		filesystem::path fsFilePath(filePath);
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		int meshCount = scene->mNumMeshes;
		if (meshCount > 0)
		{
			for (int i = 0; i < meshCount; i++)		// FBX包含的网格数量
			{
				aiMesh* fbxMesh = scene->mMeshes[i];
				Mesh* pMesh = AssetsDatabase::Instance()->AllocMesh();
				for (uint32_t j = 0; j < fbxMesh->mNumVertices; j++)
				{
					pMesh->vertices.emplace_back(Vector3(fbxMesh->mVertices[j].x, fbxMesh->mVertices[j].y, fbxMesh->mVertices[j].z));
				}

				if (fbxMesh->HasFaces())
				{
					for (uint32_t j = 0; j < fbxMesh->mNumFaces; j++)
					{
						std::vector<int> triangle(fbxMesh->mFaces[j].mNumIndices);
						std::copy(fbxMesh->mFaces[j].mIndices, fbxMesh->mFaces[j].mIndices + fbxMesh->mFaces[j].mNumIndices, triangle.begin());
						pMesh->indices.push_back(triangle);
					}
				}

				if (fbxMesh->HasVertexColors(0))
				{
					var pp = fbxMesh->mColors[0];
					for (uint32_t j = 0; j < fbxMesh->mNumVertices; j++)
					{
						pMesh->colors.emplace_back(Color(pp[j].r, pp[j].g, pp[j].b, pp[j].a));
					}
				}
				else
				{
					for (uint32_t j = 0; j < fbxMesh->mNumVertices; j++)
					{
						pMesh->colors.emplace_back(Color::White);
					}
				}
				for (int k = 0; k < 8; k++)
				{
					if (fbxMesh->HasTextureCoords(k))		// TODO: 支持UVW（Cube Map 或 Volume Map）
					{
						var pUv = fbxMesh->mTextureCoords[k];
						for (uint32_t j = 0; j < fbxMesh->mNumVertices; j++)
						{
							pMesh->uv[k].emplace_back(Vector2(pUv[j].x, pUv[j].y));
						}
					}
				}
				if (fbxMesh->HasNormals())
				{
					for (uint32_t j = 0; j < fbxMesh->mNumVertices; j++)
					{
						Vector3 norl(fbxMesh->mNormals[j].x, fbxMesh->mNormals[j].y, fbxMesh->mNormals[j].z);
						pMesh->normals.emplace_back(norl);
					}
				}
			}
		}
		else
		{
			return LoadAssetsStatusCode::NoSubMesh;
		}
		// var go = AssetsDatabase::Instance()->AllocPrefab(scene->mRootNode->mName.C_Str());
		var go = AssetsDatabase::Instance()->AllocPrefab(fsFilePath.filename().generic_string());
		var rootTrans = go->GetComponent<Transform>();
		var&& [pos, rot, scl] = AssimpMatrixToTRS(scene->mRootNode->mTransformation);
		rootTrans->LocalPosition(pos);
		rootTrans->LocalRotation(rot);
		rootTrans->LocalScale(scl);

		for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
		{
			var child = BuildGameObjectRecurate(scene->mRootNode->mChildren[i]);
			rootTrans->AddChild(child->GetComponent<Transform>());
		}
		return LoadAssetsStatusCode::OK;
	}

	LoadAssetsStatusCode LoadFbxFromFile(const string& filePath)
	{
		return LoadFbxFromFile_Internal(std::move(const_cast<string&>(filePath)));
	}

	LoadAssetsStatusCode LoadFbxFromFile(string&& filePath)
	{
		return LoadFbxFromFile_Internal(std::move(filePath));
	}
#endif
}