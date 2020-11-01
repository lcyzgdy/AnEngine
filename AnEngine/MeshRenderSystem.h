#pragma once
#ifndef __MESHRENDERSYSTEM_H__
#define __MESHRENDERSYSTEM_H__

#include "SystemBase.h"
#include "IParallel.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"

namespace AnEngine::Game::System
{
	class MeshRensererSystem : public SystemBase, public IParallel<MeshFilter, MeshRenderer>
	{
	};
}
#endif // !__MESHRENDERSYSTEM_H__
