#pragma once
#ifndef __RENDERPIPELINE_H__
#define __RENDERPIPELINE_H__

#include "RenderPipelineType.h"

namespace AnEngine::RenderCore
{
	class BaseRenderPipeline
	{
		RenderPipelineType m_rpType;
	public:
		BaseRenderPipeline() { }
		virtual ~BaseRenderPipeline() {	}

		RenderPipelineType GetRenderPipelineType() { return m_rpType; }
	};
}

#endif // !__RENDERPIPELINE_H__
