#include "NBody.h"
using namespace std;
using namespace AnEngine::RenderCore;
using namespace Microsoft::WRL;

const float NBody::ParticleSpread = 400.0f;

NBody::NBody(const HWND _hwnd, const UINT _width, const UINT _height) :
	D3D12AppBase(_hwnd, _width, _height), frameIndex(0),
	viewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height)),
	scissorRect(0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height)),
	rtvDescriptorSize(0), srvUavDescriptorSize(0),
	renderTargets{}, renderContextFenceValue(0), terminating(0), srvIndex{},
	pConstantBufferGSData(nullptr),
	fenceValues{}
{
	for (int i = 0; i < ThreadCount; i++)
	{
		renderContextFenceValues[i] = 0;
		threadFenceValues[i] = 0;
	}

	float sqRootAsyncContextsNum = sqrt(static_cast<float>(ThreadCount));
	heightInstances = static_cast<UINT>(ceil(sqRootAsyncContextsNum));
	widthInstances = static_cast<UINT>(ceil(sqRootAsyncContextsNum));
	// ceil(): 向上取整

	if (widthInstances * (heightInstances - 1) >= ThreadCount)
	{
		heightInstances--;
	}	// ??????????????
}

NBody::~NBody()
{
}

void NBody::OnInit()
{
	camera.OnInit({ 0.0f, 0.0f,1500.0f });
	camera.SetMoveSpeed(250.0f);

	InitializePipeline();
	InitializeAssets();
	CreateAsyncContexts();
}

void NBody::OnUpdate()
{
	WaitForSingleObjectEx(swapChainEvent, 100, false);
	//timer.Tick(nullptr);
	DTimer::Instance()->Tick(nullptr);
	//camera.OnUpdate(static_cast<float>(timer.GetElapsedSeconds()));
	camera.OnUpdate(static_cast<float>(DTimer::Instance()->GetElapsedSeconds()));

	ConstantBufferGS vConstantBufferGS = {};
	XMStoreFloat4x4(&vConstantBufferGS.worldViewProjection, XMMatrixMultiply(camera.GetViewMatrix(), camera.GetProjectionMatrix(0.8f, aspectRatio, 1.0f, 5000.0f)));
	XMStoreFloat4x4(&vConstantBufferGS.inverseView, XMMatrixInverse(nullptr, camera.GetViewMatrix()));

	UINT8* destination = pConstantBufferGSData + sizeof(ConstantBufferGS) * frameIndex;
	memcpy(destination, &vConstantBufferGS, sizeof(ConstantBufferGS));
}

void NBody::OnRender()
{
	for (int i = 0; i < ThreadCount; i++)
	{
		InterlockedExchange(&renderContextFenceValues[i], renderContextFenceValue);
	}
	// 让计算线程知道正在渲染新的一帧

	for (int i = 0; i < ThreadCount; i++)
	{
		UINT64 threadFenceValue = InterlockedGetValue(&threadFenceValues[i]);
		if (threadFences[i]->GetCompletedValue() < threadFenceValue)
		{
			ThrowIfFailed(commandQueue->Wait(threadFences[i].Get(), threadFenceValue));
		}
	}
	// 计算工作要在渲染开始之前完成，否则SRV会发生错误

	PopulateCommandList();
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	ThrowIfFailed(swapChain->Present(1, 0));
	MoveToNextFrame();
}

void NBody::OnRelease()
{
	InterlockedExchange(&terminating, 1);
	WaitForMultipleObjects(ThreadCount, threadHandles, TRUE, INFINITE);

	WaitForRenderContext();
	// 确保GPU不再引用将由析构函数清除的资源。

	CloseHandle(renderContextFenceEvent);
	for (int i = 0; i < ThreadCount; i++)
	{
		CloseHandle(threadHandles[i]);
		CloseHandle(threadFenceEvents[i]);
	}
}

void NBody::OnKeyUp(UINT8 _key)
{
	camera.OnKeyUp(_key);
}

void NBody::OnKeyDown(UINT8 _key)
{
	camera.OnKeyDown(_key);
}

void NBody::InitializePipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> d3dDebugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController))))
	{
		d3dDebugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	// 开启Debug模式

	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
	if (isUseWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
	}
	// 创设备

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
	// 描述并创建命令队列

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = r_DefaultFrameCount_const;
	swapChainDesc.Width = this->width;
	swapChainDesc.Height = this->height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(factory->CreateSwapChainForHwnd
	(
		commandQueue.Get(),		// 交换链需要队列，以便可以强制刷新它
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1
	));
	// 描述并创建交换链
	ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
	ThrowIfFailed(swapChain1.As(&swapChain));
	frameIndex = swapChain->GetCurrentBackBufferIndex();
	swapChainEvent = swapChain->GetFrameLatencyWaitableObject();

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = r_DefaultFrameCount_const;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));

		D3D12_DESCRIPTOR_HEAP_DESC srvUavHeapDesc = {};
		srvUavHeapDesc.NumDescriptors = DescriptorCount;
		srvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(device->CreateDescriptorHeap(&srvUavHeapDesc, IID_PPV_ARGS(&srvUavHeap)));

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		srvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}	// 创建描述符堆

	{

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV and a command allocator for each frame.
		for (UINT i = 0; i < r_DefaultFrameCount_const; i++)
		{
			ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);


			ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])));
		}
	}	// 创建帧资源
}

void NBody::InitializeAssets()
{
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
			ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
		}	// 图形根签名

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
			ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&computeRootSignature)));
		}	// 计算根签名
	}	// 创建根签名

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
		psoDesc.pRootSignature = rootSignature.Get();
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

		ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));


		D3D12_COMPUTE_PIPELINE_STATE_DESC computePsoDesc = {};
		computePsoDesc.pRootSignature = computeRootSignature.Get();
		computePsoDesc.CS = CD3DX12_SHADER_BYTECODE(computeShader.Get());

		ThrowIfFailed(device->CreateComputePipelineState(&computePsoDesc, IID_PPV_ARGS(&computeState)));
	}

	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList)));

	CreateVertexBuffer();
	CreateParticlesBuffer();

	ComPtr<ID3D12Resource> constantBufferCSUpload;

	{
		const UINT bufferSize = sizeof(ConstantBufferCS);

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&constantBufferCS)));

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBufferCSUpload)));


		ConstantBufferCS vConstantBufferCS = {};
		vConstantBufferCS.param[0] = ParticleCount;
		vConstantBufferCS.param[1] = int(ceil(ParticleCount / 128.0f));
		vConstantBufferCS.paramf[0] = 0.1f;
		vConstantBufferCS.paramf[1] = 1.0f;

		D3D12_SUBRESOURCE_DATA computeCBData = {};
		computeCBData.pData = reinterpret_cast<UINT8*>(&vConstantBufferCS);
		computeCBData.RowPitch = bufferSize;
		computeCBData.SlicePitch = computeCBData.RowPitch;

		UpdateSubresources<1>(commandList.Get(), constantBufferCS.Get(), constantBufferCSUpload.Get(), 0, 0, 1, &computeCBData);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(constantBufferCS.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}	// 创建计算着色器的常量缓冲区

	{
		const UINT constantBufferGSSize = sizeof(ConstantBufferGS) * r_DefaultFrameCount_const;

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constantBufferGSSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBufferGS)
		));

		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(constantBufferGS->Map(0, &readRange, reinterpret_cast<void**>(&pConstantBufferGSData)));
		ZeroMemory(pConstantBufferGSData, constantBufferGSSize);
	}	// 创建几何着色器的常量缓冲区

	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	{
		ThrowIfFailed(device->CreateFence(renderContextFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		renderContextFenceValue++;

		renderContextFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (renderContextFenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		WaitForRenderContext();
	}
}

void NBody::PopulateCommandList()
{
	ThrowIfFailed(commandAllocators[frameIndex]->Reset());

	ThrowIfFailed(commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get()));

	commandList->SetPipelineState(pipelineState.Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetGraphicsRootConstantBufferView(GraphicsRootCbv, constantBufferGS->GetGPUVirtualAddress() + frameIndex * sizeof(ConstantBufferGS));

	ID3D12DescriptorHeap* ppHeaps[] = { srvUavHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	commandList->RSSetScissorRects(1, &scissorRect);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	commandList->ClearRenderTargetView(rtvHandle, r_ClearColor_const_float, 0, nullptr);

	// Render the particles.
	float viewportHeight = static_cast<float>(static_cast<UINT>(viewport.Height) / heightInstances);
	float viewportWidth = static_cast<float>(static_cast<UINT>(viewport.Width) / widthInstances);
	for (UINT i = 0; i < ThreadCount; i++)
	{
		const UINT srvI = i + (srvIndex[i] == 0 ? SrvParticlePosVelo0 : SrvParticlePosVelo1);

		CD3DX12_VIEWPORT viewport(
			(i % widthInstances) * viewportWidth,
			(i / widthInstances) * viewportHeight,
			viewportWidth,
			viewportHeight);

		commandList->RSSetViewports(1, &viewport);

		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(srvUavHeap->GetGPUDescriptorHandleForHeapStart(), srvI, srvUavDescriptorSize);
		commandList->SetGraphicsRootDescriptorTable(GraphicsRootSrvTable, srvHandle);

		commandList->DrawInstanced(ParticleCount, 1, 0, 0);
	}

	commandList->RSSetViewports(1, &viewport);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(commandList->Close());
}

void NBody::WaitForGpu()
{
	commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
	// 在队列中调度信号命令。

	fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
	WaitForSingleObjectEx(fenceEvent, INFINITE, false);

	fenceValues[frameIndex]++;
}

void NBody::WaitForRenderContext()
{
	ThrowIfFailed(commandQueue->Signal(fence.Get(), renderContextFenceValue));
	// 将信号命令添加到队列中

	ThrowIfFailed(fence->SetEventOnCompletion(renderContextFenceValue, renderContextFenceEvent));
	renderContextFenceValue++;
	// 指示m_fence在信号命令完成时设置事件对象

	WaitForSingleObject(renderContextFenceEvent, INFINITE);
	// 等待直到信号命令被处理。
}

void NBody::MoveToNextFrame()
{
	fenceValues[frameIndex] = renderContextFenceValue;

	ThrowIfFailed(commandQueue->Signal(fence.Get(), renderContextFenceValue));
	renderContextFenceValue++;

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[frameIndex], renderContextFenceEvent));
		WaitForSingleObject(renderContextFenceEvent, INFINITE);
	}
}

void NBody::CreateAsyncContexts()
{
	for (UINT threadIndex = 0; threadIndex < ThreadCount; ++threadIndex)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = { D3D12_COMMAND_LIST_TYPE_COMPUTE, 0, D3D12_COMMAND_QUEUE_FLAG_NONE };
		ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&computeCommandQueue[threadIndex])));
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&computeCommandAllocators[threadIndex])));
		ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, computeCommandAllocators[threadIndex].Get(), nullptr, IID_PPV_ARGS(&computeCommandList[threadIndex])));
		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&threadFences[threadIndex])));
		// 创建计算着色器资源

		threadFenceEvents[threadIndex] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (threadFenceEvents[threadIndex] == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		threadData[threadIndex].pContext = this;
		threadData[threadIndex].threadIndex = threadIndex;

		threadHandles[threadIndex] = CreateThread(
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadProc),
			reinterpret_cast<void*>(&threadData[threadIndex]),
			CREATE_SUSPENDED,
			nullptr);

		ResumeThread(threadHandles[threadIndex]);
	}
}

void NBody::CreateVertexBuffer()
{
	vector<ParticleVertex> vertices;
	Randomize();
	vertices.resize(ParticleCount);
	for (UINT i = 0; i < ParticleCount; i++)
	{
		vertices[i].color = XMFLOAT4(Random(), Random(), Random(), 1.0f);
	}
	const UINT bufferSize = ParticleCount * sizeof(ParticleVertex);

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)));

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBufferUpload)));


	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<UINT8*>(&vertices[0]);
	vertexData.RowPitch = bufferSize;
	vertexData.SlicePitch = vertexData.RowPitch;

	UpdateSubresources<1>(commandList.Get(), vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast<UINT>(bufferSize);
	vertexBufferView.StrideInBytes = sizeof(ParticleVertex);
}

float NBody::RandomPercent()
{
	float ret = static_cast<float>((rand() % 10000) - 5000);
	return ret / 5000.0f;
}

void NBody::InitializeParticles(Particle * _pParticles, const XMFLOAT3 & _center, const XMFLOAT4 & _velocity, float _spread, UINT _particlesNum)
{
	Randomize();
	for (UINT i = 0; i < _particlesNum; i++)
	{
		XMFLOAT3 delta(_spread, _spread, _spread);

		while (XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&delta))) > _spread * _spread)
		{
			delta.x = RandomPercent() * _spread;
			delta.y = RandomPercent() * _spread;
			delta.z = RandomPercent() * _spread;
		}

		_pParticles[i].position.x = _center.x + delta.x;
		_pParticles[i].position.y = _center.y + delta.y;
		_pParticles[i].position.z = _center.z + delta.z;
		_pParticles[i].position.w = 10000.0f * 10000.0f;

		_pParticles[i].velocity = _velocity;
	}
}

void NBody::CreateParticlesBuffer()
{
	vector<Particle> data;
	data.resize(ParticleCount);
	const UINT dataSize = ParticleCount * sizeof(Particle);
	// 缓冲区初始化数据

	float centerSpread = ParticleSpread * 0.50f;
	InitializeParticles(&data[0], XMFLOAT3(centerSpread, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, -20.0f, 1.0f / 100000000.0f), ParticleSpread, ParticleCount / 2);
	InitializeParticles(&data[ParticleCount / 2], XMFLOAT3(-centerSpread, 0, 0), XMFLOAT4(0, 0, 20, 1 / 100000000.0f), ParticleSpread, ParticleCount / 2);
	// 将粒子分成两组。

	D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	D3D12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);

	for (UINT index = 0; index < ThreadCount; index++)
	{
		// 在GPU中创建两个缓冲区，每个都有一个粒子数据的副本。
		// 计算着色器将更新其中的一个，而渲染线程渲染另一个。 当渲染完成时，线程将交换它们工作的缓冲区。

		ThrowIfFailed(device->CreateCommittedResource(
			&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&particleBuffer0[index])));

		ThrowIfFailed(device->CreateCommittedResource(
			&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&particleBuffer1[index])));

		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&particleBuffer0Upload[index])));

		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&particleBuffer1Upload[index])));

		D3D12_SUBRESOURCE_DATA particleData = {};
		particleData.pData = reinterpret_cast<UINT8*>(&data[0]);
		particleData.RowPitch = dataSize;
		particleData.SlicePitch = particleData.RowPitch;

		UpdateSubresources<1>(commandList.Get(), particleBuffer0[index].Get(), particleBuffer0Upload[index].Get(), 0, 0, 1, &particleData);
		UpdateSubresources<1>(commandList.Get(), particleBuffer1[index].Get(), particleBuffer1Upload[index].Get(), 0, 0, 1, &particleData);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(particleBuffer0[index].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(particleBuffer1[index].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = ParticleCount;
		srvDesc.Buffer.StructureByteStride = sizeof(Particle);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle0(srvUavHeap->GetCPUDescriptorHandleForHeapStart(), SrvParticlePosVelo0 + index, srvUavDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle1(srvUavHeap->GetCPUDescriptorHandleForHeapStart(), SrvParticlePosVelo1 + index, srvUavDescriptorSize);
		device->CreateShaderResourceView(particleBuffer0[index].Get(), &srvDesc, srvHandle0);
		device->CreateShaderResourceView(particleBuffer1[index].Get(), &srvDesc, srvHandle1);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = ParticleCount;
		uavDesc.Buffer.StructureByteStride = sizeof(Particle);
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE uavHandle0(srvUavHeap->GetCPUDescriptorHandleForHeapStart(), UavParticlePosVelo0 + index, srvUavDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE uavHandle1(srvUavHeap->GetCPUDescriptorHandleForHeapStart(), UavParticlePosVelo1 + index, srvUavDescriptorSize);
		device->CreateUnorderedAccessView(particleBuffer0[index].Get(), nullptr, &uavDesc, uavHandle0);
		device->CreateUnorderedAccessView(particleBuffer1[index].Get(), nullptr, &uavDesc, uavHandle1);
	}
}

DWORD WINAPI NBody::ThreadProc(ThreadData * _pThread)
{
	return _pThread->pContext->AsyncComputeThreadProc(_pThread->threadIndex);
}

DWORD NBody::AsyncComputeThreadProc(int _threadIndex)
{
	ID3D12CommandQueue* pCommandQueue = computeCommandQueue[_threadIndex].Get();
	ID3D12CommandAllocator* pCommandAllocator = computeCommandAllocators[_threadIndex].Get();
	ID3D12GraphicsCommandList* pCommandList = computeCommandList[_threadIndex].Get();
	ID3D12Fence* pFence = threadFences[_threadIndex].Get();

	while (0 == InterlockedGetValue(&terminating))
	{
		Simulate(_threadIndex);
		// 运行粒子

		ThrowIfFailed(pCommandList->Close());
		ID3D12CommandList* ppCommandLists[] = { pCommandList };

		pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

		UINT64 threadFenceValue = InterlockedIncrement(&threadFenceValues[_threadIndex]);
		ThrowIfFailed(pCommandQueue->Signal(pFence, threadFenceValue));
		ThrowIfFailed(pFence->SetEventOnCompletion(threadFenceValue, threadFenceEvents[_threadIndex]));
		WaitForSingleObject(threadFenceEvents[_threadIndex], INFINITE);
		// 等待计算着色器完成。

		UINT64 renderContextFenceValue = InterlockedGetValue(&renderContextFenceValues[_threadIndex]);
		if (fence->GetCompletedValue() < renderContextFenceValue)
		{
			ThrowIfFailed(pCommandQueue->Wait(fence.Get(), renderContextFenceValue));
			InterlockedExchange(&renderContextFenceValues[_threadIndex], 0);
		}
		// 等待渲染线程使用SRV完成，以便下一帧可以运行。

		srvIndex[_threadIndex] = 1 - srvIndex[_threadIndex];
		// SRV和UAV中的索引互换

		ThrowIfFailed(pCommandAllocator->Reset());
		ThrowIfFailed(pCommandList->Reset(pCommandAllocator, computeState.Get()));
		// 准备下一帧
	}

	return 0;
}

void NBody::Simulate(UINT _threadIndex)
{
	ID3D12GraphicsCommandList* pCommandList = computeCommandList[_threadIndex].Get();

	UINT vSrvIndex;
	UINT vUavIndex;
	ID3D12Resource *pUavResource;
	if (srvIndex[_threadIndex] == 0)
	{
		vSrvIndex = SrvParticlePosVelo0;
		vUavIndex = UavParticlePosVelo1;
		pUavResource = particleBuffer1[_threadIndex].Get();
	}
	else
	{
		vSrvIndex = SrvParticlePosVelo1;
		vUavIndex = UavParticlePosVelo0;
		pUavResource = particleBuffer0[_threadIndex].Get();
	}

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pUavResource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	pCommandList->SetPipelineState(computeState.Get());
	pCommandList->SetComputeRootSignature(computeRootSignature.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { srvUavHeap.Get() };
	pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(srvUavHeap->GetGPUDescriptorHandleForHeapStart(), vSrvIndex + _threadIndex, srvUavDescriptorSize);
	CD3DX12_GPU_DESCRIPTOR_HANDLE uavHandle(srvUavHeap->GetGPUDescriptorHandleForHeapStart(), vUavIndex + _threadIndex, srvUavDescriptorSize);

	pCommandList->SetComputeRootConstantBufferView(ComputeRootCbv, constantBufferCS->GetGPUVirtualAddress());
	pCommandList->SetComputeRootDescriptorTable(ComputeRootSrvTable, srvHandle);
	pCommandList->SetComputeRootDescriptorTable(ComputeRootUavTable, uavHandle);

	pCommandList->Dispatch(static_cast<int>(ceil(ParticleCount / 128.0f)), 1, 1);

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pUavResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

}
