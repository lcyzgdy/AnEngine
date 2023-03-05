#pragma once
#ifndef __ANENGINE_INTERFACE__COMMANDLIST_HPP__
#define __ANENGINE_INTERFACE__COMMANDLIST_HPP__

#include <memory>

#include "IGpuDevice.hpp"

namespace AnEngine::Rendering
{
	class ICommandList
	{
	public:
		virtual void Create(const IPlatformGpuDevice* gpuContext) = 0;
	};
}

#endif // !__ANENGINE_INTERFACE__COMMANDLIST_HPP__
