#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "ObjectBehaviour.h"
#include "PipelineState.h"
#include "ColorBuffer.h"
#include "GpuBuffer.h"
#include "ShaderClass.h"
#include <future>

namespace AnEngine::RenderCore::Resource
{
	class ColorBuffer;
}

namespace AnEngine::Game
{
	class Scene;

	class Renderer : public ObjectBehaviour
	{
		std::future<void> m_renderTask;
	protected:
		RenderCore::Resource::ColorBuffer* m_renderTarget;
		RenderCore::GraphicPSO* m_pso;
		RenderCore::RootSignature* m_rootSignature;

		virtual void Start() override;

		//virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void LateUpdate() override;

		virtual void LoadAsset() = 0;
		virtual void OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator) = 0;

	public:
		//explicit Renderer(const std::wstring& name);
		//explicit Renderer(std::wstring&& name);
		Renderer();
		virtual ~Renderer() = default;

		virtual void Destory() override;
	};

	class TrangleRender : public Renderer
	{
		::AnEngine::RenderCore::Resource::ByteAddressBuffer* m_vertexBuffer;
		::AnEngine::RenderCore::VertexShader* m_vertexShader;
		::AnEngine::RenderCore::PixelShader* m_pixelShader;
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

	protected:
		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator) override;

	public:
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		/*TrangleRender(const std::wstring& name);
		TrangleRender(std::wstring&& name);*/
		TrangleRender();

		virtual void Destory() override;
	};
}

#endif // !__RENDERER_H__
