#include "PipelineState.h"
using namespace RenderCore;

PipelineStateObject::PipelineStateObject() : m_rootSignature(nullptr), m_pipelineState(nullptr)
{
}

ID3D12PipelineState * PipelineStateObject::GetPSO() const
{
	return m_pipelineState;
}

const RootSignature & PipelineStateObject::GetRootSignature()
{
	if (m_rootSignature == nullptr) throw exception();
	return *m_rootSignature;
}

void PipelineStateObject::SetRootSignature(const RootSignature & rootSignature)
{
	m_rootSignature = &rootSignature;
}

RenderCore::GraphicPSO::GraphicPSO()
{
	memset(&m_psoDesc, 0, sizeof(m_psoDesc));
	m_psoDesc.NodeMask = 1;
	m_psoDesc.SampleMask = 0xffffffffu;
	m_psoDesc.SampleDesc.Count = 1;
	m_psoDesc.InputLayout.NumElements = 0;
}

void RenderCore::GraphicPSO::SetBlendState(const D3D12_BLEND_DESC & blendDesc)
{
	m_psoDesc.BlendState = blendDesc;
}

void RenderCore::GraphicPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC & rasterizerDesc)
{
	m_psoDesc.RasterizerState = rasterizerDesc;
}

void RenderCore::GraphicPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC & depthStencilDesc)
{
	m_psoDesc.DepthStencilState = depthStencilDesc;
}

void RenderCore::GraphicPSO::SetSampleMask(uint32_t sampleMask)
{
	m_psoDesc.SampleMask = sampleMask;
}

void RenderCore::GraphicPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
{
	m_psoDesc.PrimitiveTopologyType = topologyType;
}

void RenderCore::GraphicPSO::SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, uint32_t msaaCount, uint32_t msaaQuality)
{
	SetRenderTargetFormats(1, &rtvFormat, dsvFormat, msaaCount, msaaQuality);
}

void RenderCore::GraphicPSO::SetRenderTargetFormats(uint32_t rtvNum, const DXGI_FORMAT * rtvFormats, DXGI_FORMAT dsvFormat, uint32_t msaaCount, UINT msaaQuality)
{
	if (rtvNum <= 0 || rtvFormats == nullptr) ERRORBLOCK("RTV error");
	for (size_t i = 0; i < rtvNum; i++)
	{
		m_psoDesc.RTVFormats[i] = rtvFormats[i];
	}
	for (size_t i = rtvNum; i < m_psoDesc.NumRenderTargets; i++)
	{
		m_psoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	}
	m_psoDesc.NumRenderTargets = rtvNum;
	m_psoDesc.DSVFormat = dsvFormat;
	m_psoDesc.SampleDesc.Count = msaaCount;
	m_psoDesc.SampleDesc.Quality = msaaQuality;
}

void RenderCore::GraphicPSO::SetInputLayout(uint32_t elementsNum, const D3D12_INPUT_ELEMENT_DESC * p_inputElementDescs)
{
	m_psoDesc.InputLayout.NumElements = elementsNum;
	if (elementsNum == 0)
	{
		m_psoDesc.InputLayout.pInputElementDescs = nullptr;
	}
	else
	{
		D3D12_INPUT_ELEMENT_DESC* newElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * elementsNum);
		memcpy(newElements, p_inputElementDescs, elementsNum * sizeof(D3D12_INPUT_ELEMENT_DESC));
		m_inputLayouts.reset(static_cast<const D3D12_INPUT_ELEMENT_DESC*>(newElements));
		// 拷贝一份，以防原数据更改后导致PSO失效
	}
}

void RenderCore::GraphicPSO::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps)
{
	m_psoDesc.IBStripCutValue = indexBufferProps;
}

void RenderCore::GraphicPSO::SetVertexShader(void * vertexBinary, size_t size)
{
}
