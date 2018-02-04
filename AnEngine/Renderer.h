#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include"ObjectBehaviour.h"
#include"PipelineState.h"

namespace AnEngine::RenderCore::Resource
{
	class ColorBuffer;
}

namespace AnEngine::Game
{
	class Scene;

	class Renderer : public ObjectBehaviour
	{
	protected:
		::AnEngine::RenderCore::Resource::ColorBuffer* m_renderTarget;
		GraphicPSO m_pso;

	public:
		Renderer();
		~Renderer();

		virtual void Start() override;

		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;

		virtual void LoadAsset() = 0;
		virtual void OnRender() = 0;
	};

	class TrangleRender : public Renderer
	{
	public:
		// Í¨¹ý Renderer ¼Ì³Ð
		virtual void LoadAsset() override;
		virtual void OnRender() override;
	};
}

#endif // !__RENDERER_H__
