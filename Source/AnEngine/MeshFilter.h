#pragma once
#ifndef __MESHFILTER_H__
#define __MESHFILTER_H__

#include "Component.h"
#include "Mesh.h"

namespace AnEngine::Game
{
	class MeshFilter : public ComponentBase
	{
		const Resource::Mesh* m_sharedMesh;
	};
}

#endif // !__MESHFILTER_H__
