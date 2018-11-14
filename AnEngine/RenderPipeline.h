#pragma once
#ifndef __RENDERPIPELINE_H__
#define __RENDERPIPELINE_H__

#include "CommandContext.h"
#include "FenceContext.h"
#include "RenderPipelineType.h"
#include "onwind.h"
#include <mutex>

namespace AnEngine::RenderCore
{
	class BaseRenderPipeline
	{
		RenderPipelineType m_rpType;
	public:
		explicit BaseRenderPipeline(RenderPipelineType type) : m_rpType(type) { }
		virtual ~BaseRenderPipeline() {	}

		virtual void OnRender(std::mutex&) = 0;
		RenderPipelineType GetRenderPipelineType() { return m_rpType; }
	};
}

#endif // !__RENDERPIPELINE_H__
