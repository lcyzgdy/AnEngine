#include "PipelineState.h"
#include "Hash.hpp"
#include <mutex>
#include "RenderCore.h"
using namespace std;
using namespace AnEngine::RenderCore;

namespace AnEngine::RenderCore
{
	PipelineStateObject::PipelineStateObject() : m_pipelineState(nullptr)
	{
	}

	ID3D12PipelineState* PipelineStateObject::GetPSO()
	{
		return m_pipelineState.Get();
	}

	ID3D12PipelineState** PipelineStateObject::operator&()
	{
		return &m_pipelineState;
	}

	/*RootSignature* PipelineStateObject::GetRootSignature()
	{
		return &m_rootSignature;
	}

	void PipelineStateObject::SetRootSignature(const RootSignature& rootSignature)
	{
		m_rootSignature = rootSignature;
	}*/

	GraphicPSO::GraphicPSO()
	{
		memset(&m_psoDesc, 0, sizeof(m_psoDesc));
		m_psoDesc.NodeMask = 0;
		m_psoDesc.SampleMask = 0xffffffffu;
		m_psoDesc.SampleDesc.Count = 1;
		m_psoDesc.InputLayout.NumElements = 0;
	}

	void GraphicPSO::SetRootSignature(ID3D12RootSignature * rootSignature)
	{
		m_psoDesc.pRootSignature = rootSignature;
	}

	void GraphicPSO::SetBlendState(const D3D12_BLEND_DESC& blendDesc)
	{
		m_psoDesc.BlendState = blendDesc;
	}

	void GraphicPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc)
	{
		m_psoDesc.RasterizerState = rasterizerDesc;
	}

	void GraphicPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
	{
		m_psoDesc.DepthStencilState = depthStencilDesc;
	}

	void GraphicPSO::SetDepthStencilState(bool depthEnable, bool stencilEnable)
	{
		m_psoDesc.DepthStencilState.DepthEnable = depthEnable;
		m_psoDesc.DepthStencilState.StencilEnable = stencilEnable;
	}

	void GraphicPSO::SetSampleMask(uint32_t sampleMask)
	{
		m_psoDesc.SampleMask = sampleMask;
	}

	void GraphicPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
	{
		m_psoDesc.PrimitiveTopologyType = topologyType;
	}

	void GraphicPSO::SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, uint32_t msaaCount, uint32_t msaaQuality)
	{
		SetRenderTargetFormats(1, &rtvFormat, dsvFormat, msaaCount, msaaQuality);
	}

	void GraphicPSO::SetRenderTargetFormats(uint32_t rtvNum, const DXGI_FORMAT * rtvFormats, DXGI_FORMAT dsvFormat, uint32_t msaaCount, UINT msaaQuality)
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

	void GraphicPSO::SetInputLayout(uint32_t elementsNum, const D3D12_INPUT_ELEMENT_DESC* p_inputElementDescs)
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

			m_psoDesc.InputLayout.pInputElementDescs = &(*m_inputLayouts);
		}
	}

	void GraphicPSO::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps)
	{
		m_psoDesc.IBStripCutValue = indexBufferProps;
	}

	void GraphicPSO::SetVertexShader(const void * binary, size_t size)
	{
		m_psoDesc.VS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(binary), size);
	}

	void GraphicPSO::SetPixelShader(const void * binary, size_t size)
	{
		m_psoDesc.PS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(binary), size);
	}

	void GraphicPSO::SetGeometryShader(const void * binary, size_t size)
	{
		m_psoDesc.GS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(binary), size);
	}

	void GraphicPSO::SetHullShader(const void * binary, size_t size)
	{
		m_psoDesc.HS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(binary), size);
	}

	void GraphicPSO::SetDomainShader(const void * binary, size_t size)
	{
		m_psoDesc.DS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(binary), size);
	}

	void GraphicPSO::SetVertexShader(const D3D12_SHADER_BYTECODE & binary)
	{
		m_psoDesc.VS = binary;
	}

	void GraphicPSO::SetPixelShader(const D3D12_SHADER_BYTECODE & binary)
	{
		m_psoDesc.PS = binary;
	}

	void GraphicPSO::SetGeometryShader(const D3D12_SHADER_BYTECODE & binary)
	{
		m_psoDesc.GS = binary;
	}

	void GraphicPSO::SetHullShader(const D3D12_SHADER_BYTECODE & binary)
	{
		m_psoDesc.HS = binary;
	}

	void GraphicPSO::SetDomainShader(const D3D12_SHADER_BYTECODE & binary)
	{
		m_psoDesc.DS = binary;
	}

	void GraphicPSO::Finalize()
	{
		var device = r_graphicsCard[0]->GetDevice();
		ThrowIfFailed(device->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)), R_GetGpuError);
	}

	void GraphicPSO::Finalize(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
	{
		/*m_psoDesc.BlendState.AlphaToCoverageEnable = desc.BlendState.AlphaToCoverageEnable;
		m_psoDesc.BlendState.IndependentBlendEnable = desc.BlendState.IndependentBlendEnable;
		for (int i = 0; i < 8; i++) m_psoDesc.BlendState.RenderTarget[i] = desc.BlendState.RenderTarget[i];
		m_psoDesc.CachedPSO = desc.CachedPSO;
		m_psoDesc.DepthStencilState = desc.DepthStencilState;
		m_psoDesc.DS = desc.DS;
		m_psoDesc.DSVFormat = desc.DSVFormat;
		m_psoDesc.Flags = desc.Flags;
		m_psoDesc.GS = desc.GS;
		m_psoDesc.HS = desc.HS;
		m_psoDesc.IBStripCutValue = desc.IBStripCutValue;
		SetInputLayout(desc.InputLayout.NumElements, desc.InputLayout.pInputElementDescs);
		m_psoDesc.NodeMask = desc.NodeMask;
		m_psoDesc.NumRenderTargets = desc.NumRenderTargets;
		m_psoDesc.PrimitiveTopologyType = desc.PrimitiveTopologyType;
		m_psoDesc.pRootSignature = desc.pRootSignature;
		m_psoDesc.PS = desc.PS;
		m_psoDesc.RasterizerState = desc.RasterizerState;
		for (int i = 0; i < 8; i++) m_psoDesc.RTVFormats[i] = desc.RTVFormats[i];
		m_psoDesc.SampleDesc = desc.SampleDesc;
		m_psoDesc.SampleMask = desc.SampleMask;
		m_psoDesc.StreamOutput = desc.StreamOutput;
		m_psoDesc.VS = desc.VS;*/
		m_psoDesc = desc;

		var device = r_graphicsCard[0]->GetDevice();
		ThrowIfFailed(device->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)));
	}


	/*--------------------------==============================----------------------------------*/


	ComputePSO::ComputePSO()
	{
		ZeroMemory(&m_psoDesc, sizeof(m_psoDesc));
		m_psoDesc.NodeMask = 1;
	}

	void ComputePSO::SetRootSignature(ID3D12RootSignature* rootSignature)
	{
		m_psoDesc.pRootSignature = rootSignature;
	}

	void ComputePSO::SetComputeShader(const void* binary, size_t size)
	{
		m_psoDesc.CS = CD3DX12_SHADER_BYTECODE(binary, size);
	}

	void ComputePSO::SetComputeShader(const D3D12_SHADER_BYTECODE& binary)
	{
		m_psoDesc.CS = binary;
	}

	void ComputePSO::Finalize()
	{
		//m_psoDesc.pRootSignature = m_rootSignature->GetRootSignature();
		var device = r_graphicsCard[0]->GetDevice();
		if (m_psoDesc.pRootSignature == nullptr) ERRORBREAK("compute_rootsignature");

		/*size_t hashCode = Utility::GetHash(&m_psoDesc);

		ID3D12PipelineState** psoRef = nullptr;
		bool firstComplie = false;
		{
			static mutex s_hashMapMutex;
			lock_guard<mutex> lock(s_hashMapMutex);

			if (var iter = r_s_computePSOMap.find(hashCode); iter == r_s_computePSOMap.end())
			{
				firstComplie = true;
				psoRef = r_s_computePSOMap[hashCode].GetAddressOf();
			}
			else
			{
				psoRef = iter->second.GetAddressOf();
			}
		}
		if (firstComplie)
		{
			ThrowIfFailed(device->CreateComputePipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)));
			r_s_computePSOMap[hashCode].Attach(m_pipelineState.Get());
		}
		else
		{
			while (*psoRef == nullptr)
			{
				this_thread::yield();
			}
			m_pipelineState = *psoRef;
		}*/

		ThrowIfFailed(device->CreateComputePipelineState(&m_psoDesc, IID_PPV_ARGS(&m_pipelineState)));
	}
}