#pragma once
#ifdef __USE_RAYTRACING__
#ifndef __RAYTRACINGHLSLCOMPAT_H__
#define __RAYTRACINGHLSLCOMPAT_H__

namespace AnEngine::RenderCore
{
	struct Viewport
	{
		float left;
		float top;
		float right;
		float bottom;
	};

	struct RayGenConstantBuffer
	{
		Viewport viewport;
		Viewport stencil;
	};
}

#endif // __RAYTRACINGHLSLCOMPAT_H__
#endif // __USE_RAYTRACING__