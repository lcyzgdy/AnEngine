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
	SampleMeshRenderer::SampleMeshRenderer(wstring && fileName) : Renderer(), m_fileName(fileName)
	{
		m_pso = new GraphicPSO();
	}

	void SampleMeshRenderer::LoadAsset()
	{
		var device = r_graphicsCard[0]->GetDevice();
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();
		var iAllocator = commandAllocator->GetAllocator();
		iAllocator->Reset();
		iList->Reset(iAllocator, nullptr);

		var[m_dsvHeap, m_dsvHandle, m_dsvGpuHandle] = DescriptorHeapAllocator::GetInstance()->Allocate2(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		const uint32_t nullSrvCount = 2;
		const uint32_t cbvCount = 2;
		const uint32_t srvCount = _countof(SampleAssets::Textures);
		var[m_cbvSrvHeap, m_cbvSrvHandle, m_cbvSrvGpuHandle] = DescriptorHeapAllocator::GetInstance()->Allocate2(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			nullSrvCount + cbvCount + srvCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		var[m_samplerHeap, m_samplerHandle, m_samplerGpuHandle] = DescriptorHeapAllocator::GetInstance()->Allocate2(
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		{
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}

			CD3DX12_DESCRIPTOR_RANGE1 ranges[4];
			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 两个SRV用于漫反射贴图和法线贴图，使用t1和t2
			ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 一个CBV用于常量
			ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);												// 一个SRV用于阴影
			ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2, 0);											// 两个静态采样器

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
			uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			uint32_t compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

			ThrowIfFailed(D3DCompileFromFile((wstring(SOLUTION_DIR) + L"\\Assets\\shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
			ThrowIfFailed(D3DCompileFromFile((wstring(SOLUTION_DIR) + L"\\Assets\\shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

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
		ThrowIfFailed(ReadDataFromFile(m_fileName.c_str(), &pAssetData, &fileSize));

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

		uint32_t cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(m_cbvSrvHandle);
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuHandle(m_cbvSrvGpuHandle);
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC nullSrvDesc = {};
			nullSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			nullSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			nullSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			nullSrvDesc.Texture2D.MipLevels = 1;
			nullSrvDesc.Texture2D.MostDetailedMip = 0;
			nullSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

			device->CreateShaderResourceView(nullptr, &nullSrvDesc, cbvSrvHandle);
			cbvSrvHandle.Offset(cbvSrvDescriptorSize);

			device->CreateShaderResourceView(nullptr, &nullSrvDesc, cbvSrvHandle);
			cbvSrvHandle.Offset(cbvSrvDescriptorSize);
		}
		//uint32_t srvCount = _countof(SampleAssets::Textures);

		for (int i = 0; i < srvCount; i++)
		{
			const SampleAssets::TextureResource &tex = SampleAssets::Textures[i];
			CD3DX12_RESOURCE_DESC texDesc(D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, tex.Width, tex.Height, 1, static_cast<uint16_t>(tex.MipLevels),
				tex.Format, 1, 0, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE);

			ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&texDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_textures[i])));

			const uint32_t subresourceCount = texDesc.DepthOrArraySize * texDesc.MipLevels;
			uint64_t uploadBufferSize = GetRequiredIntermediateSize(m_textures[i].Get(), 0, subresourceCount);
			ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
				IID_PPV_ARGS(&m_textureUploads[i])));

			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData = pAssetData + tex.Data->Offset;
			textureData.RowPitch = tex.Data->Pitch;
			textureData.SlicePitch = tex.Data->Size;

			UpdateSubresources(iList, m_textures[i].Get(), m_textureUploads[i].Get(), 0, 0, subresourceCount, &textureData);
			iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_textures[i].Get(), D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = tex.Format;
			srvDesc.Texture2D.MipLevels = tex.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			device->CreateShaderResourceView(m_textures[i].Get(), &srvDesc, cbvSrvHandle);

			cbvSrvHandle.Offset(cbvSrvDescriptorSize);
		}
		free(pAssetData);

		{
			uint32_t samplerDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			CD3DX12_CPU_DESCRIPTOR_HANDLE samplerHandle(m_samplerHandle);

			D3D12_SAMPLER_DESC wrapSamplerDesc = {};
			wrapSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			wrapSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			wrapSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			wrapSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			wrapSamplerDesc.MinLOD = 0;
			wrapSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			wrapSamplerDesc.MipLODBias = 0.0f;
			wrapSamplerDesc.MaxAnisotropy = 1;
			wrapSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			wrapSamplerDesc.BorderColor[0] = wrapSamplerDesc.BorderColor[1] = wrapSamplerDesc.BorderColor[2] = wrapSamplerDesc.BorderColor[3] = 0;
			device->CreateSampler(&wrapSamplerDesc, samplerHandle);

			// Move the handle to the next slot in the descriptor heap.
			samplerHandle.Offset(samplerDescriptorSize);

			// Describe and create the point clamping sampler, which is 
			// used for the shadow map.
			D3D12_SAMPLER_DESC clampSamplerDesc = {};
			clampSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			clampSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			clampSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			clampSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			clampSamplerDesc.MipLODBias = 0.0f;
			clampSamplerDesc.MaxAnisotropy = 1;
			clampSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			clampSamplerDesc.BorderColor[0] = clampSamplerDesc.BorderColor[1] = clampSamplerDesc.BorderColor[2] = clampSamplerDesc.BorderColor[3] = 0;
			clampSamplerDesc.MinLOD = 0;
			clampSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			device->CreateSampler(&clampSamplerDesc, samplerHandle);
		}

		for (int i = 0; i < NumLights; i++)
		{
			m_lights[i].position = { 0.0f, 15.0f, -30.0f, 1.0f };
			m_lights[i].direction = { 0.0, 0.0f, 1.0f, 0.0f };
			m_lights[i].falloff = { 800.0f, 1.0f, 0.0f, 1.0f };
			m_lights[i].color = { 0.7f, 0.7f, 0.7f, 1.0f };

			XMVECTOR eye = XMLoadFloat4(&m_lights[i].position);
			XMVECTOR at = XMVectorAdd(eye, XMLoadFloat4(&m_lights[i].direction));
			XMVECTOR up = { 0, 1, 0 };

			m_lightCameras[i].Set(eye, at, up);
		}

		ThrowIfFailed(iList->Close());
		//ID3D12CommandList* ppCommandLists[] = { iList };
		//r_graphicsCard[0]->ExecuteSync(_countof(ppCommandLists), ppCommandLists);

		CD3DX12_RESOURCE_DESC shadowTexDesc(D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, static_cast<UINT>(Screen::GetInstance()->Width()),
			static_cast<UINT>(Screen::GetInstance()->Height()), 1, 1, DXGI_FORMAT_R32_TYPELESS, 1, 0, D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&shadowTexDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_shadowTexture)));

		uint32_t constantBufferSize = (sizeof(MyConstantBuffer) +
			(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&m_shadowConstantBuffer)));
		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&m_constantBuffer)));

		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(m_shadowConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mp_shadowConstantBufferWO)));
		ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mp_sceneConstantBufferWO)));

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.SizeInBytes = constantBufferSize;
		cbvDesc.BufferLocation = m_shadowConstantBuffer->GetGPUVirtualAddress();

		device->CreateConstantBufferView(&cbvDesc, cbvSrvHandle);
		m_shadowCbvHandle = cbvSrvGpuHandle;

		cbvSrvHandle.Offset(cbvSrvDescriptorSize);
		cbvSrvGpuHandle.Offset(cbvSrvDescriptorSize);

		GraphicsContext::Push(commandList, commandAllocator);

		//m_fence = new Fence(r_graphicsCard[0]->GetCommandQueue());
		m_fence = new Fence();
	}

	void SampleMeshRenderer::OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator)
	{
		var[shadowList, shadowAllocator] = GraphicsContext::GetOne();
		var iShadowAllocator = shadowAllocator->GetAllocator();
		var iShadowList = shadowList->GetCommandList();

		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, m_pso->GetPSO()));

		ThrowIfFailed(iShadowAllocator->Reset());
		ThrowIfFailed(iShadowList->Reset(iShadowAllocator, m_psoShadowMap->GetPSO()));

		// Graphics
		{
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GetRTV();
			iList->ClearDepthStencilView(m_dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			var commonToRenderTarget = CommonState::commonToRenderTarget;
			var renderTargetToCommon = CommonState::renderTargetToCommon;
			commonToRenderTarget.Transition.pResource = m_renderTarget->GetResource();
			renderTargetToCommon.Transition.pResource = m_renderTarget->GetResource();
			iList->ResourceBarrier(1, &commonToRenderTarget);

			iList->SetGraphicsRootSignature(m_rootSignature.Get());
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvHeap, m_samplerHeap };
			iList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			iList->RSSetViewports(1, &m_viewport);
			iList->RSSetScissorRects(1, &m_scissorRect);
			iList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			iList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetVbv());
			iList->IASetIndexBuffer(&m_vertexBuffer->GetIbv());
			iList->SetGraphicsRootDescriptorTable(3, m_samplerGpuHandle);
			iList->OMSetStencilRef(0);

			CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHandle);
			iList->SetGraphicsRootDescriptorTable(2, m_shadowDepthHandle);
			iList->SetGraphicsRootDescriptorTable(1, m_cbvGpuHandle);

			iList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
			D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvHeapStart = m_cbvGpuHandle;
			uint32_t cbvSrvDescriptorSize = r_graphicsCard[0]->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			uint32_t nullSrvCount = 2;
			for (int j = 0; j < _countof(SampleAssets::Draws); j++)
			{
				SampleAssets::DrawParameters drawArgs = SampleAssets::Draws[j];

				CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvHandle(cbvSrvHeapStart, nullSrvCount + drawArgs.DiffuseTextureIndex, cbvSrvDescriptorSize);
				iList->SetGraphicsRootDescriptorTable(0, cbvSrvHandle);

				iList->DrawIndexedInstanced(drawArgs.IndexCount, 1, drawArgs.IndexStart, drawArgs.VertexBase, 0);
			}
			iList->ResourceBarrier(1, &renderTargetToCommon);
		}
		// Shadow
		{
			var depthWriteToPsResource = CommonState::depthWrite2PsResource;
			var psResourceToDepthWrite = CommonState::psResource2DepthWrite;
			depthWriteToPsResource.Transition.pResource = m_depthStencil.Get();
			psResourceToDepthWrite.Transition.pResource = m_depthStencil.Get();
			iShadowList->ResourceBarrier(1, &depthWriteToPsResource);

			iShadowList->SetGraphicsRootSignature(m_rootSignature.Get());

			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvHeap, m_samplerHeap };
			iShadowList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			iShadowList->RSSetViewports(1, &m_viewport);
			iShadowList->RSSetScissorRects(1, &m_scissorRect);
			iShadowList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			iShadowList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetVbv());
			iShadowList->IASetIndexBuffer(&m_vertexBuffer->GetIbv());
			iShadowList->SetGraphicsRootDescriptorTable(3, m_samplerHeap->GetGPUDescriptorHandleForHeapStart());
			iShadowList->OMSetStencilRef(0);

			iShadowList->SetGraphicsRootDescriptorTable(2, m_nullSrvHandle);			// Set a null SRV for the shadow texture.
			iShadowList->SetGraphicsRootDescriptorTable(1, m_shadowCbvHandle);

			iShadowList->OMSetRenderTargets(0, nullptr, FALSE, &m_dsvHandle);

			iShadowList->SetGraphicsRootDescriptorTable(0, m_cbvGpuHandle);
			for (int i = 0; i < _countof(SampleAssets::Draws); i++)
			{
				SampleAssets::DrawParameters drawArgs = SampleAssets::Draws[i];
				iShadowList->DrawIndexedInstanced(drawArgs.IndexCount, 1, drawArgs.IndexStart, drawArgs.VertexBase, 0);
			}

			iShadowList->ResourceBarrier(1, &psResourceToDepthWrite);
		}

		iList->Close();
		iShadowList->Close();

		//ID3D12CommandList* ppcommandList[] = { iShadowList };
		//r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);
		//m_fence->GpuSignal(0);

		GraphicsContext::Push(shadowList, shadowAllocator);
	}

	void SampleMeshRenderer::Destory()
	{
		Renderer::Destory();
		delete m_psoShadowMap;
		delete m_vertexBuffer;
	}
}