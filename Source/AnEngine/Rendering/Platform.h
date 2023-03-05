#pragma once
#ifndef __ANENGINE_RENDERING__PLATFORM_H__
#define __ANENGINE_RENDERING__PLATFORM_H__

namespace AnEngine::Rendering
{
	enum class GraphicsAPI
	{
		Unknown,
		Software,
		Direct3D12,
		// Direct3D11,
		// OpenGL,
		// Vulkan,
	};
}

#endif // !__ANENGINE_RENDERING__PLATFORM_H__
