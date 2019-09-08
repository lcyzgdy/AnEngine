#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#include "onwind.h"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Color.h"

namespace AnEngine::AssetsWrapper
{
	std::byte* LoadFbxFromFile(const std::wstring&);
}

namespace AnEngine::Resource
{
	class Mesh
	{
		friend std::byte* AnEngine::AssetsWrapper::LoadFbxFromFile(const std::wstring&);

		std::vector<DMath::Vector3> m_vertices;
		std::vector<DMath::Vector3> m_normals;
		std::vector<uint32_t> m_triangles;

		std::vector<DMath::Vector2> m_uv;
		std::vector<DMath::Vector2> m_uv2;
		std::vector<DMath::Vector2> m_uv3;
		std::vector<DMath::Vector2> m_uv4;
		std::vector<DMath::Vector2> m_uv5;
		std::vector<DMath::Vector2> m_uv6;
		std::vector<DMath::Vector2> m_uv7;
		std::vector<DMath::Vector2> m_uv8;

		std::vector<float> m_boneWeights;
		std::vector<DMath::Matrix4x4> m_bindPose;


		std::vector<Color> m_colors;

	public:
		uint32_t VerticesCount() { return m_vertices.size(); }
	};
}

#endif // !__MESH_H__
