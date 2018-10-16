#pragma once
#ifndef __DEFERREDRENDERPIPELINE_H__
#define __DEFERREDRENDERPIPELINE_H__

#include "RenderPipeline.h"

namespace AnEngine::RenderCore
{
	class DeferredRenderPipeline : public BaseRenderPipeline
	{
		// 通过 BaseRenderPipeline 继承
		virtual void OnRender() override;
		void GBuffer();
		void DepthPreLight();
	public:
		DeferredRenderPipeline() : BaseRenderPipeline(RenderPipelineType::Deferred) {}
	};
}

#endif // !__DEFERREDRENDERPIPELINE_H__
