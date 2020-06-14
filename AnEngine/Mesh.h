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
	class Mesh : public Object
	{
	public:
		std::vector<DMath::Vector3> vertices;
		std::vector<DMath::Vector3> normals;
		std::vector<uint32_t> triangles;

		std::vector<DMath::Vector2> uv[8];

		std::vector<float> boneWeights;
		std::vector<DMath::Matrix4x4> bindPose;


		std::vector<Color> colors;

	public:
		inline uint32_t VerticesCount() { return vertices.size(); }
	};
}

#endif // !__MESH_H__
