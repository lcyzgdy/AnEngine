#include "ParticlesRenderer.h"
#include "RenderCore.h"
#include "CommandContext.h"
#include "Screen.h"
#include "DTimer.h"
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::Game
{
	void ParticlesRenderer::Simulate()
	{
		var[commandList, commandAllocator] = ComputeContext::GetOne();
		var iList = commandList->GetCommandList();
		var iAllocator = commandAllocator->GetAllocator();
		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, m_computePso.Get()));

		var[vSrvIndex, vUavIndex, pUavResource] = m_particles->GetSrvIndexAndUavResource();

		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pUavResource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		iList->SetPipelineState(m_computePso.Get());
		iList->SetComputeRootSignature(m_computeRootSignature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = { m_particles->GetDescriptorHeap() };
		iList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		var[srvUavHandle, srvUavGpuHandle] = m_particles->GetSrvHandle();
		uint32_t srvUavDescriptorSize = r_graphicsCard[0]->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(srvUavGpuHandle, vSrvIndex, srvUavDescriptorSize);
		CD3DX12_GPU_DESCRIPTOR_HANDLE uavHandle(srvUavGpuHandle, vUavIndex, srvUavDescriptorSize);

		iList->SetComputeRootConstantBufferView(ComputeRootCbv, m_constantBufferCS->GetGPUVirtualAddress());
		iList->SetComputeRootDescriptorTable(ComputeRootSrvTable, srvHandle);
		iList->SetComputeRootDescriptorTable(ComputeRootUavTable, uavHandle);

		iList->Dispatch(static_cast<int>(ceil(m_particles->GetParticleCount() / 128.0f)), 1, 1);

		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pUavResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

		ThrowIfFailed(iList->Close());

		//ID3D12CommandList* ppList[] = { iList };
		//r_graphicsCard[0]->ExecuteSync(_countof(ppList), ppList);

		GraphicsContext::Push(commandList, commandAllocator);
	}

	void ParticlesRenderer::WaitForRenderContext()
	{
		ThrowIfFailed(m_graphicsQueue->Signal(m_srvUavFence.Get(), m_srvUavFenceValue));

		ThrowIfFailed(m_srvUavFence->SetEventOnCompletion(m_srvUavFenceValue, m_srvUavFenceEvent));
		m_srvUavFenceValue++;
		// 指示m_fence在信号命令完成时设置事件对象

		WaitForSingleObject(m_srvUavFenceEvent, INFINITE);
	}

	ParticlesRenderer::ParticlesRenderer() : Renderer(),
		m_viewport(0.0f, 0.0f, static_cast<float>(Screen::GetInstance()->Width()), static_cast<float>(Screen::GetInstance()->Height())),
		m_scissorRect(0, 0, static_cast<long>(Screen::GetInstance()->Width()), static_cast<long>(Screen::GetInstance()->Height()))
	{
		m_graphicsQueue = r_graphicsCard[0]->GetCommandQueue();
		m_computeQueue = r_graphicsCard[0]->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	}

	void ParticlesRenderer::LoadAsset()
	{
		var device = r_graphicsCard[0]->GetDevice();
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();
		var iAllocator = commandAllocator->GetAllocator();

		var[computeList, computeAllocator] = ComputeContext::GetOne();
		var cList = computeList->GetCommandList();
		var cAllocator = computeAllocator->GetAllocator();

		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, nullptr));
		ThrowIfFailed(cAllocator->Reset());
		ThrowIfFailed(cList->Reset(cAllocator, nullptr));

		m_particles = new SampleParticles();

		{
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}

			{
				CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
				ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

				CD3DX12_ROOT_PARAMETER1 rootParameters[GraphicsRootParametersCount];
				rootParameters[GraphicsRootCbv].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
				rootParameters[GraphicsRootSrvTable].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
				rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				ComPtr<ID3DBlob> signature;
				ComPtr<ID3DBlob> error;
				ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
				ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
					IID_PPV_ARGS(&m_rootSignature1)));
			}

			{
				CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
				ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
				ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

				CD3DX12_ROOT_PARAMETER1 rootParameters[ComputeRootParametersCount];
				rootParameters[ComputeRootCbv].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
				rootParameters[ComputeRootSrvTable].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
				rootParameters[ComputeRootUavTable].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);

				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSignatureDesc;
				computeRootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr);

				ComPtr<ID3DBlob> signature;
				ComPtr<ID3DBlob> error;
				ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&computeRootSignatureDesc, featureData.HighestVersion, &signature, &error));
				ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
					IID_PPV_ARGS(&m_computeRootSignature)));
			}
		}
		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> geometryShader;
			ComPtr<ID3DBlob> pixelShader;
			ComPtr<ID3DBlob> computeShader;

#if defined(_DEBUG)
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT compileFlags = 0;
#endif

			ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"ParticleDraw.hlsl").c_str(), nullptr, nullptr, "VSParticleDraw", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
			ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"ParticleDraw.hlsl").c_str(), nullptr, nullptr, "GSParticleDraw", "gs_5_0", compileFlags, 0, &geometryShader, nullptr));
			ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"ParticleDraw.hlsl").c_str(), nullptr, nullptr, "PSParticleDraw", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
			ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"NBodyGravityCS.hlsl").c_str(), nullptr, nullptr, "CSMain", "cs_5_0", compileFlags, 0, &computeShader, nullptr));

			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = m_rootSignature1.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.GS = CD3DX12_SHADER_BYTECODE(geometryShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = blendDesc;
			psoDesc.DepthStencilState = depthStencilDesc;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			psoDesc.SampleDesc.Count = 1;
			psoDesc.NodeMask = 1;

			m_pso = new GraphicPSO();
			m_pso->Finalize(psoDesc);

			D3D12_COMPUTE_PIPELINE_STATE_DESC computePsoDesc = {};
			computePsoDesc.pRootSignature = m_computeRootSignature.Get();
			computePsoDesc.CS = CD3DX12_SHADER_BYTECODE(computeShader.Get());
			ThrowIfFailed(device->CreateComputePipelineState(&computePsoDesc, IID_PPV_ARGS(&m_computePso)));
		}

		ComPtr<ID3D12Resource> constantBufferCSUpload;
		{
			const UINT bufferSize = sizeof(ConstantBufferCS);

			ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_constantBufferCS)));
			ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constantBufferCSUpload)));

			ConstantBufferCS vConstantBufferCS = {};
			vConstantBufferCS.param[0] = m_particles->GetParticleCount();
			vConstantBufferCS.param[1] = int(ceil(m_particles->GetParticleCount() / 128.0f));
			vConstantBufferCS.paramf[0] = 0.1f;
			vConstantBufferCS.paramf[1] = 1.0f;

			D3D12_SUBRESOURCE_DATA computeCBData = {};
			computeCBData.pData = reinterpret_cast<UINT8*>(&vConstantBufferCS);
			computeCBData.RowPitch = bufferSize;
			computeCBData.SlicePitch = computeCBData.RowPitch;

			UpdateSubresources<1>(cList, m_constantBufferCS.Get(), constantBufferCSUpload.Get(), 0, 0, 1, &computeCBData);
			cList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_constantBufferCS.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
		}	// 创建计算着色器的常量缓冲区
		{
			uint32_t constantBufferGSSize = sizeof(ConstantBufferGS) * r_DefaultFrameCount_const;

			ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(constantBufferGSSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
				IID_PPV_ARGS(&m_constantBufferGS)));

			CD3DX12_RANGE readRange(0, 0);
			ThrowIfFailed(m_constantBufferGS->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstantBufferGSData)));
			ZeroMemory(m_pConstantBufferGSData, constantBufferGSSize);
		}	// 创建几何着色器的常量缓冲区ThrowIfFailed(commandList->Close());

		{
			ThrowIfFailed(iList->Close(), R_GetGpuError);
			//ID3D12CommandList* ppCommandLists[] = { iList };
			//r_graphicsCard[0]->ExecuteSync(_countof(ppCommandLists), ppCommandLists);
		}
		{
			ThrowIfFailed(cList->Close(), R_GetGpuError);
			//ID3D12CommandList* ppCommandList[] = { cList };
			//r_graphicsCard[0]->ExecuteSync(_countof(ppCommandList), ppCommandList, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		}

		//m_fence = new Fence(r_graphicsCard[0]->GetCommandQueue());
		//m_fence = new Fence();
		{
			ThrowIfFailed(device->CreateFence(m_srvUavFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_srvUavFence)));
			m_srvUavFenceValue++;

			m_srvUavFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_srvUavFenceEvent == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}

			WaitForRenderContext();
		}

		GraphicsContext::Push(commandList, commandAllocator);
		ComputeContext::Push(computeList, computeAllocator);
	}

	void ParticlesRenderer::OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator)
	{
		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, m_pso->GetPSO()));

		m_graphicsQueue->Wait(m_srvUavFence.Get(), m_srvUavFenceValue);

		iList->SetPipelineState(m_pso->GetPSO());
		iList->SetGraphicsRootSignature(m_rootSignature1.Get());
		iList->SetGraphicsRootConstantBufferView(GraphicsRootCbv, m_constantBufferGS->GetGPUVirtualAddress());

		ID3D12DescriptorHeap* ppHeaps[] = { m_particles->GetDescriptorHeap() };
		iList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		iList->IASetVertexBuffers(0, 1, &m_particles->GetVertexBuffer()->GetVbv());
		iList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		iList->RSSetScissorRects(1, &m_scissorRect);

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToCommon;
		commonToRenderTarget.Transition.pResource = m_renderTarget->GetResource();
		renderTargetToCommon.Transition.pResource = m_renderTarget->GetResource();

		iList->ResourceBarrier(1, &commonToRenderTarget);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_renderTarget->GetRTV());
		iList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		iList->ClearRenderTargetView(rtvHandle, r_ClearColor_const_float, 0, nullptr);

		float viewportHeight = static_cast<float>(static_cast<uint32_t>(m_viewport.Height));
		float viewportWidth = static_cast<float>(static_cast<uint32_t>(m_viewport.Width));

		iList->RSSetViewports(1, &m_viewport);
		var[srvUavHandle, srvUavGpuHandle] = m_particles->GetSrvHandle();
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(srvUavGpuHandle);
		iList->SetGraphicsRootDescriptorTable(GraphicsRootSrvTable, srvHandle);
		iList->DrawInstanced(m_particles->GetParticleCount(), 1, 0, 0);
		//iList->RSSetViewports(1, &m_viewport);

		iList->ResourceBarrier(1, &renderTargetToCommon);
		ThrowIfFailed(iList->Close());

		//ID3D12CommandList* ppcommandList[] = { iList };
		//r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);
		//m_renderTarget->GetFence()->GpuSignal(0);
		m_srvUavFenceValue++;
		ThrowIfFailed(m_graphicsQueue->Signal(m_srvUavFence.Get(), m_srvUavFenceValue));
	}

	void ParticlesRenderer::Update()
	{
		m_camera.OnUpdate(static_cast<float>(DTimer::GetInstance()->GetElapsedSeconds()));

		ConstantBufferGS vConstantBufferGS = {};
		XMStoreFloat4x4(&vConstantBufferGS.worldViewProjection, XMMatrixMultiply(m_camera.GetViewMatrix(),
			m_camera.GetProjectionMatrix(0.8f, 1.2f, 1.0f, 5000.0f)));
		XMStoreFloat4x4(&vConstantBufferGS.inverseView, XMMatrixInverse(nullptr, m_camera.GetViewMatrix()));

		uint8_t* destination = m_pConstantBufferGSData + sizeof(ConstantBufferGS);
		memcpy(destination, &vConstantBufferGS, sizeof(ConstantBufferGS));

		ThrowIfFailed(m_computeQueue->Wait(m_srvUavFence.Get(), m_srvUavFenceValue));
		Simulate();
		m_particles->SwapSrvUavIndex();
		m_srvUavFenceValue++;
		ThrowIfFailed(m_computeQueue->Signal(m_srvUavFence.Get(), m_srvUavFenceValue));
	}

	void ParticlesRenderer::Destory()
	{
		Renderer::Destory();
		delete m_particles;
	}
}