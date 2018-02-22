#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "ObjectBehaviour.h"
#include "PipelineState.h"
#include "ColorBuffer.h"
#include "GpuBuffer.h"
#include "ShaderClass.h"

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
		::AnEngine::RenderCore::GraphicPSO* m_pso;
		::AnEngine::RenderCore::RootSignature* m_rootSignature;

		virtual void Start() override;

		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;

	public:
		Renderer(const std::wstring& name);
		Renderer(std::wstring&& name);
		virtual ~Renderer() = default;


		virtual void LoadAsset() = 0;
		virtual void OnRender() = 0;
	};

	class TrangleRender : public Renderer
	{
		::AnEngine::RenderCore::Resource::ByteAddressBuffer* m_vertexBuffer;
		::AnEngine::RenderCore::VertexShader* m_vertexShader;
		::AnEngine::RenderCore::PixelShader* m_pixelShader;
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

	public:
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		TrangleRender(const std::wstring& name);
		TrangleRender(std::wstring&& name);

		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender() override;

		virtual void Destory() override;
	};
}

#endif // !__RENDERER_H__
