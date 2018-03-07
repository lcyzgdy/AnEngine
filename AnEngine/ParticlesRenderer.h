#pragma once
#ifndef __PARTICLESRENDERER_H__
#define __PARTICLESRENDERER_H__

#include"Renderer.h"
#include"VertexBuffer.h"
#include"SampleParticles.h"
#include"Fence.h"

namespace AnEngine::Game
{
	class ParticlesRenderer : public Renderer
	{
		struct ConstantBufferCS
		{
			uint32_t param[4];
			float paramf[4];
		};
		struct ConstantBufferGS
		{
			XMFLOAT4X4 worldViewProjection;
			XMFLOAT4X4 inverseView;

			float padding[32];
			// 常量缓冲区在显存中以256字节对齐。
		};

		enum GraphicsRootParameters : uint32_t
		{
			GraphicsRootCbv = 0,
			GraphicsRootSrvTable,
			GraphicsRootParametersCount
		};

		enum ComputeRootParameters : uint32_t
		{
			ComputeRootCbv = 0,
			ComputeRootSrvTable,
			ComputeRootUavTable,
			ComputeRootParametersCount
		};

		RenderCore::Resource::SampleParticles* m_particles;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_computeRootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_computePso;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBufferGS;
		std::byte* m_pConstantBufferGSData;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBufferCS;

		RenderCore::Fence* m_fence;

		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;
		D3D12_RECT m_cullingScissorRect;	// 用作遮挡剔除

	public:
		ParticlesRenderer(std::wstring&& name);
		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator) override;

		virtual void Destory() override;
	};
}

#endif // !__PARTICLESRENDERER_H__
