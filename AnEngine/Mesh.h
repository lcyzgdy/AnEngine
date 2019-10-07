#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#include "onwind.h"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Color.h"
#include "AssetsConfig.h"

namespace AnEngine::AssetsWrapper
{
	extern LoadAssetsStatusCode LoadFbxFromFile(const std::string&);
	extern LoadAssetsStatusCode LoadFbxFromFile(std::string&&);
}

namespace AnEngine::Resource
{
	class Mesh
	{
		friend AssetsWrapper::LoadAssetsStatusCode AnEngine::AssetsWrapper::LoadFbxFromFile(const std::string&);
		friend AssetsWrapper::LoadAssetsStatusCode AnEngine::AssetsWrapper::LoadFbxFromFile(std::string&&);

		std::vector<DMath::Vector3> m_vertices;
		std::vector<DMath::Vector3> m_normals;
		std::vector<uint32_t> m_triangles;

		std::vector<DMath::Vector2> m_uv[8];

		std::vector<float> m_boneWeights;
		std::vector<DMath::Matrix4x4> m_bindPose;


		std::vector<Color> m_colors;

	public:
		inline uint32_t VerticesCount() { return m_vertices.size(); }
	};
}

#endif // !__MESH_H__
