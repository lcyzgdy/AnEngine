#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include"ObjectBehaviour.h"

namespace AnEngine::RenderCore::Resource
{
	class ColorBuffer;
}

namespace AnEngine::Game
{
	class Scene;

	class Renderer : public ObjectBehaviour
	{
		::AnEngine::RenderCore::Resource::ColorBuffer* m_renderTarget;
	public:
		Renderer();
		~Renderer();

		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;

		virtual void OnRender() = 0;
	};

	class TrangleRender : public Renderer
	{
	public:
		// Í¨¹ý Renderer ¼Ì³Ð
		virtual void OnRender() override;
	};
}

#endif // !__RENDERER_H__
