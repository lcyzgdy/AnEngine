#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#define OPENFBX

#include "onwind.h"
#include "Vector.hpp"

#if defined OPENFBX

#include "ofbx.h"

#endif

namespace AnEngine::Assets
{
	class Mesh
	{
		std::vector<DMath::Vector4> m_vertices;
		std::vector<uint32_t> m_triangles;
		std::vector<DMath::Vector3> m_normals;

	public:
		Mesh(const std::wstring& filePath);
	};
}

#endif // !__MESH_H__
