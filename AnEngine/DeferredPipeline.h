#pragma once
#ifndef __DEFERREDPIPELINE_H__
#define __DEFERREDPIPELINE_H__

#include "ColorBuffer.h"
#include "DepthBuffer.h"

namespace AnEngine::RenderCore
{
	class DeferredPipeline
	{
		Resource::ColorBuffer* m_GBuffer0;
		Resource::DepthBuffer* m_CameraDepth;
	public:
		void DrawGBuffer();
	};
}

#endif // !__DEFERREDPIPELINE_H__
