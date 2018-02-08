#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include"ObjectBehaviour.h"
#include"PipelineState.h"
#include"ColorBuffer.h"
#include"GpuBuffer.h"

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
		GraphicPSO* m_pso;
		RootSignature* m_rootSignature;

	public:
		Renderer();
		virtual ~Renderer() = default;

		virtual void Start() override;

		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;

		virtual void LoadAsset() = 0;
		virtual void OnRender() = 0;
	};

	class TrangleRender : public Renderer
	{
		Resource::ByteAddressBuffer* m_vertexBuffer;
	public:
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender() override;

		virtual void Destory() override;
	};
}

#endif // !__RENDERER_H__
