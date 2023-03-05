#pragma once
#ifndef __ANENGINE__RENDERINGFACTORY_H__
#define __ANENGINE__RENDERINGFACTORY_H__

#include "CommandList.h"

namespace AnEngine::Rendering
{
	class RenderingFactory
	{
	public:
		static RenderingFactory* GetInstance();

		CommandList CreateCommandList();
	};
}

#endif // !__ANENGINE__RENDERINGFACTORY_H__
