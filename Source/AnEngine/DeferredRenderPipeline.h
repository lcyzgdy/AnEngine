#pragma once
#ifndef __DEFERREDRENDERPIPELINE_H__
#define __DEFERREDRENDERPIPELINE_H__

#include "RenderPipeline.h"

namespace AnEngine::RenderCore
{
	class DeferredRenderPipeline : public BaseRenderPipeline
	{
		void GBuffer();
		void DepthPreLight();
		void Shadow();
		void UI();
		void PostProcess();
	public:
		DeferredRenderPipeline() : BaseRenderPipeline(RenderPipelineType::Deferred) {}
		// 通过 BaseRenderPipeline 继承
		virtual void OnRender(std::mutex& sceneResMutex) override;
	};
}

#endif // !__DEFERREDRENDERPIPELINE_H__
