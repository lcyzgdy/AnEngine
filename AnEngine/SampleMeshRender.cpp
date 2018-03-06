#include "SampleMeshRender.h"
#include "RenderCore.h"
#include "Screen.h"
#include "DescriptorHeap.hpp"
#include "CommandContext.h"
using namespace AnEngine::RenderCore;
using namespace AnEngine;
using namespace AnEngine::RenderCore::Heap;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::Game
{
	SampleMeshRenderer::SampleMeshRenderer(wstring&& name, wstring && fileName) : Renderer(name), m_fileName(fileName)
	{
	}

	void SampleMeshRenderer::LoadAsset()
	{
		var device = r_graphicsCard[0]->GetDevice();
		var[m_dsvHeap, m_dsvHandle] = DescriptorHeapAllocator::GetInstance()->Allocate2(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		var[commandList, commandAllocator] = G

		{
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[4]; // Perfomance TIP: Order from most frequent to least frequent.
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 2 frequently changed diffuse + normal textures - using registers t1 and t2.
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 1 frequently changed constant buffer.
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);												// 1 infrequently changed shadow texture - starting in register t0.
		ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2, 0);											// 2 static samplers.

		CD3DX12_ROOT_PARAMETER1 rootParameters[4];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[3].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSignature)));
		}
		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
			// Enable better shader debugging with the graphics debugging tools.
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

			ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"..\\Assets\\shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
			ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"..\\Assets\\shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

			D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
			inputLayoutDesc.pInputElementDescs = SampleAssets::StandardVertexDescription;
			inputLayoutDesc.NumElements = _countof(SampleAssets::StandardVertexDescription);

			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			depthStencilDesc.StencilEnable = FALSE;

			// Describe and create the PSO for rendering the scene.
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = inputLayoutDesc;
			psoDesc.pRootSignature = m_rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState = depthStencilDesc;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleDesc.Count = 1;

			m_pso->Finalize(psoDesc);

			// Alter the description and create the PSO for rendering
			// the shadow map.  The shadow map does not use a pixel
			// shader or render targets.
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(0, 0);
			psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			psoDesc.NumRenderTargets = 0;

			m_psoShadowMap = new GraphicPSO();
			m_psoShadowMap->Finalize(psoDesc);
		}
		{
			CD3DX12_RESOURCE_DESC shadowTextureDesc(D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, static_cast<uint32_t>(Screen::GetInstance()->Width()),
				static_cast<uint32_t>(Screen::GetInstance()->Height()), 1, 1, DXGI_FORMAT_D32_FLOAT, 1, 0, D3D12_TEXTURE_LAYOUT_UNKNOWN,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

			D3D12_CLEAR_VALUE clearValue;
			clearValue.Format = DXGI_FORMAT_D32_FLOAT;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;

			ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&shadowTextureDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_depthStencil)));

			device->CreateDepthStencilView(m_depthStencil.Get(), nullptr, m_dsvHandle);
		}
		uint32_t fileSize = 0;
		uint8_t* pAssetData;
		ThrowIfFailed(ReadDataFromFile(GetAssetFullPath(m_fileName).c_str(), &pAssetData, &fileSize));

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = pAssetData + SampleAssets::VertexDataOffset;
		vertexData.RowPitch = SampleAssets::VertexDataSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = pAssetData + SampleAssets::IndexDataOffset;
		indexData.RowPitch = SampleAssets::IndexDataSize;
		indexData.SlicePitch = indexData.RowPitch;

		m_vertexBuffer = new VertexBuffer(vertexData, SampleAssets::VertexDataSize, SampleAssets::StandardVertexStride,
			indexData, SampleAssets::IndexDataSize, SampleAssets::StandardIndexFormat);
	}

	void SampleMeshRenderer::OnRender()
	{
	}

	void SampleMeshRenderer::Destory()
	{
		Renderer::Destory();
		delete m_psoShadowMap;
		delete m_vertexBuffer;
	}
}