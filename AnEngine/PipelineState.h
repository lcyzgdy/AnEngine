#pragma once
#ifndef __PIPELINESTATE_H__
#define __PIPELINESTATE_H__
#include"DX.h"
#include"RootSignature.h"
#include<map>


namespace AnEngine::RenderCore
{
	static map<uint32_t, ComPtr<ID3D12PipelineState>> r_s_graphicPSOMap;
	static map<uint32_t, ComPtr<ID3D12PipelineState>> r_s_computePSOMap;

	typedef class PipelineStateObject PSO;

	class PipelineStateObject
	{
	protected:
		const RootSignature* m_rootSignature;
		ID3D12PipelineState* m_pipelineState;

	public:
		PipelineStateObject();
		~PipelineStateObject() = default;

		ID3D12PipelineState* GetPSO() const;
		const RootSignature& GetRootSignature();
		void SetRootSignature(const RootSignature& rootSignature);
	};

	class GraphicPSO : public PSO
	{
	protected:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
		shared_ptr<const D3D12_INPUT_ELEMENT_DESC> m_inputLayouts;
	public:
		GraphicPSO();
		~GraphicPSO() = default;

		void SetBlendState(const D3D12_BLEND_DESC& blendDesc);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);
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

		void Finalize(ID3D12Device* device);
	};

	class ComputePSO : public PSO
	{
	protected:
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_psoDesc;
		const RootSignature* m_rootSignature;
	public:
		ComputePSO();
		~ComputePSO() = default;

		void SetComputeShader(const void* binary, size_t size);
		void SetComputeShader(const D3D12_SHADER_BYTECODE& binary);

		void Finalize(ID3D12Device* device);
	};
}
#endif // !__PIPELINESTATE_H__
