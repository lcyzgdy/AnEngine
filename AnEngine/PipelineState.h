#pragma once
#ifndef __PIPELINESTATE_H__
#define __PIPELINESTATE_H__
#include"DX.h"
#include"RootSignature.h"
#include<map>

namespace AnEngine::RenderCore
{
	class PipelineStateObject
	{
	protected:
		//RootSignature m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

	public:
		PipelineStateObject();
		~PipelineStateObject() = default;

		ID3D12PipelineState* GetPSO();

		ID3D12RootSignature* GetRootSignature() { return m_rootSignature.Get(); }
		void SetRootSignature(ID3D12RootSignature* rootSignature);

		ID3D12PipelineState** operator&() { return &m_pipelineState; }
		//RootSignature* GetRootSignature();
		//void SetRootSignature(const RootSignature& rootSignature);
	};

	class GraphicPSO : public PipelineStateObject
	{
	protected:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
		std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> m_inputLayouts;
	public:
		GraphicPSO();
		~GraphicPSO() = default;

		//virtual void SetRootSignature(ID3D12RootSignature* rootSignature) override;
		void SetBlendState(const D3D12_BLEND_DESC& blendDesc);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);
		void SetDepthStencilState(bool DepthEnable, bool StencilEnable);
		void SetSampleMask(uint32_t sampleMask);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
		void SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, uint32_t msaaCount = 1, uint32_t msaaQuality = 0);
		void SetRenderTargetFormats(uint32_t rtvNum, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, uint32_t msaaCount = 1, UINT msaaQuality = 0);
		void SetInputLayout(uint32_t elementsNum, const D3D12_INPUT_ELEMENT_DESC* p_inputElementDescs);
		void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps);

		void SetVertexShader(const void* binary, size_t size);
		void SetPixelShader(const void* binary, size_t size);
		void SetGeometryShader(const void* binary, size_t size);
		void SetHullShader(const void* binary, size_t size);
		void SetDomainShader(const void* binary, size_t size);

		void SetVertexShader(const D3D12_SHADER_BYTECODE& binary);
		void SetPixelShader(const D3D12_SHADER_BYTECODE& binary);
		void SetGeometryShader(const D3D12_SHADER_BYTECODE& binary);
		void SetHullShader(const D3D12_SHADER_BYTECODE& binary);
		void SetDomainShader(const D3D12_SHADER_BYTECODE& binary);

		void Finalize();
		void Finalize(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

		static D3D12_RASTERIZER_DESC RastrizerDesc_Opaque;
	};

	class ComputePSO : public PipelineStateObject
	{
	protected:
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_psoDesc;
	public:
		ComputePSO();
		~ComputePSO() = default;

		//virtual void SetRootSignature(ID3D12RootSignature* rootSignature) override;
		void SetComputeShader(const void* binary, size_t size);
		void SetComputeShader(const D3D12_SHADER_BYTECODE& binary);

		void Finalize();
	};
}
#endif // !__PIPELINESTATE_H__
