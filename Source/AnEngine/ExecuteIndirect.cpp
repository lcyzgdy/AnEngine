#include "ExecuteIndirect.h"
#include <string>

using namespace Microsoft::WRL;
using namespace DirectX;

const uint32_t ExecuteIndirect::commandSizePerFrame = triangleCount * sizeof(IndirectCommand);
const uint32_t ExecuteIndirect::commandBufferCounterOffset = AlignForUavCounter(ExecuteIndirect::commandSizePerFrame);
const float ExecuteIndirect::triangleHalfWidth = 0.05f;
const float ExecuteIndirect::triangleDepth = 1.0f;
const float ExecuteIndirect::cullingCutoff = 0.5f;

ExecuteIndirect::ExecuteIndirect(const HWND _hwnd, const uint32_t _width, const uint32_t _height, const wstring& _title) :
	D3D12AppBase(_hwnd, _width, _height, _title), pCbvDataBegin(nullptr),
	constantBufferData{}, csRootConstants(), enableCulling(true),
	viewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height)),
	scissorRect(0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height)),
	rtvDescriptorSize(0), cbvSrvUavDescriptorSize(0),
	fenceValues{}, renderTargets{}
{
	constantBufferData.resize(triangleCount);

	csRootConstants.xOffset = triangleHalfWidth;
	csRootConstants.zOffset = triangleDepth;
	csRootConstants.cullOffset = cullingCutoff;
	csRootConstants.commandCount = triangleCount;

	float center = width / 2.0f;
	cullingScissorRect.left = static_cast<LONG>(center - (center * cullingCutoff));
	cullingScissorRect.right = static_cast<LONG>(center + (center * cullingCutoff));
	cullingScissorRect.bottom = static_cast<LONG>(height);
}

ExecuteIndirect::~ExecuteIndirect()
{
}

void ExecuteIndirect::OnInit()
{
	InitializePipeline();
	InitializeAssets();
}

void ExecuteIndirect::OnUpdate()
{
	for (uint32_t i = 0; i < triangleCount; i++)
	{
		const float offsetBounds = 2.5f;

		constantBufferData[i].offset.x += constantBufferData[i].velocity.x;
		if (constantBufferData[i].offset.x > offsetBounds)
		{
			constantBufferData[i].velocity.x = Random(0.01f, 0.02f);
			constantBufferData[i].offset.x = -offsetBounds;
		}
	}

	uint8_t* destination = pCbvDataBegin + (triangleCount * frameIndex * sizeof(SceneConstantBuffer));
	memcpy(destination, &constantBufferData[0], triangleCount * sizeof(SceneConstantBuffer));
	// 将数据拷贝到场景常量缓冲区
}

void ExecuteIndirect::OnRender()
{
	PopulateCommandList();

	if (enableCulling)
	{
		//PIXBeginEvent(commandQueue.Get(), 0, L"Cull invisible triangles");

		ID3D12CommandList* ppCommandLists[] = { computeCommandList.Get() };
		computeCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		//PIXEndEvent(commandQueue.Get());

		computeCommandQueue->Signal(computeFence.Get(), fenceValues[frameIndex]);

		commandQueue->Wait(computeFence.Get(), fenceValues[frameIndex]);
		// 仅当计算机工作完成时才执行渲染工作
	}
	//PIXBeginEvent(commandQueue.Get(), 0, L"Render");	// ??????

	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	// 执行渲染工作

	//PIXEndEvent(commandQueue.Get());

	swapChain->Present(1, 0);
	MoveToNextFrame();
}

void ExecuteIndirect::OnRelease()
{
	WaitForGpu();
	CloseHandle(fenceEvent);
}

void ExecuteIndirect::OnKeyUp(uint8_t key)
{
}

void ExecuteIndirect::OnKeyDown(uint8_t key)
{
	if (key == VK_SPACE)
	{
		enableCulling = !enableCulling;
	}
}

void ExecuteIndirect::InitializePipeline()
{
	uint32_t dxgiFactoryFlags = 0;

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

	D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	ThrowIfFailed(device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeCommandQueue)));
	// 描述并创建计算着色器的命令队列

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = r_DefaultFrameCount_const;
	swapChainDesc.Width = this->width;
	swapChainDesc.Height = this->height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
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

	//#1
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = r_DefaultFrameCount_const;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
	// 创建渲染目标视图描述符堆

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));
	// 描述和创建深度模板视图[depth stencil view（DSV）]描述符堆

	// Describe and create a constant buffer view (CBV), Shader resource
	// view (SRV), and unordered access view (UAV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
	cbvSrvUavHeapDesc.NumDescriptors = CbvSrvUavDescriptorCountPerFrame * r_DefaultFrameCount_const;
	cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(device->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&cbvSrvUavHeap)));
	// 描述并创建常量缓冲视图【constant buffer view (CBV)】、着色器资源视图【Shader resource
	// view (SRV)】、无序访问视图【unordered access view (UAV)】描述符堆
	/*
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	//srvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	// 描述并创建纹理的着色器资源视图堆 [Shader resource view (SRV) heap]

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvHeap));
	// 描述并创建一个常量缓冲区视图（CBV）描述符堆。
	// 标志指示此描述符堆可以绑定到流水线，并且其中包含的描述符可以由根堆引用。
	*/
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// !#1	创建描述符堆

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (uint32_t i = 0; i < r_DefaultFrameCount_const; i++)
	{
		ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);

		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])));
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&computeCommandAllocators[i])));
		//// !!!!
	}
	// 创建命令分配器
	// 创建帧资源
}

void ExecuteIndirect::InitializeAssets()
{
	// #1
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_ROOT_PARAMETER1 rootParameters[GraphicsRootParametersCount];
		rootParameters[Cbv].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		// 允许输入汇编输入布局
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
		auto hr2 = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		//创建根签名

		CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

		CD3DX12_ROOT_PARAMETER1 computeRootParameters[ComputeRootParametersCount];
		computeRootParameters[SrvUavTable].InitAsDescriptorTable(2, ranges);
		computeRootParameters[RootConstants].InitAsConstants(4, 0);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSignatureDesc;
		computeRootSignatureDesc.Init_1_1(_countof(computeRootParameters), computeRootParameters);

		D3DX12SerializeVersionedRootSignature(&computeRootSignatureDesc, featureData.HighestVersion, &signature, &error);
		device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&computeRootSignature));
		// 创建计算着色器根签名
		// !#1
	}
	// #2
	{
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;
		ComPtr<ID3DBlob> computeShader;
		ComPtr<ID3DBlob> error;

#if defined(_DEBUG)
		uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		uint32_t compileFlags = 0;
#endif

		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"execute_indirect_shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error));
		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"execute_indirect_shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error));
		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"compute_shader.hlsl").c_str(), nullptr, nullptr, "CSMain", "cs_5_0", compileFlags, 0, &computeShader, &error));

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		// Describe and create the graphics pipeline state objects (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

		D3D12_COMPUTE_PIPELINE_STATE_DESC computePsoDesc = {};
		computePsoDesc.pRootSignature = computeRootSignature.Get();
		computePsoDesc.CS = CD3DX12_SHADER_BYTECODE(computeShader.Get());

		device->CreateComputePipelineState(&computePsoDesc, IID_PPV_ARGS(&computeState));
		// !#2	// 创建管线状态，包括编译和加载着色器
	}

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, computeCommandAllocators[frameIndex].Get(), computeState.Get(), IID_PPV_ARGS(&computeCommandList));
	computeCommandList->Close();
	// 创建渲染命令链表和计算命令链表

	ComPtr<ID3D12Resource> vertexBufferUpload;
	ComPtr<ID3D12Resource> commandBufferUpload;
	// #3
	{
		Vertex triangleVertices[] =
		{
			{ { 0.0f, triangleHalfWidth, triangleDepth } },
			{ { triangleHalfWidth, -triangleHalfWidth, triangleDepth } },
			{ { -triangleHalfWidth, -triangleHalfWidth, triangleDepth } }
		};
		const uint32_t vertexBufferSize = sizeof(triangleVertices);
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer));
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBufferUpload));

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<uint8_t*>(triangleVertices);
		vertexData.RowPitch = vertexBufferSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources<1>(commandList.Get(), vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = sizeof(triangleVertices);
		// !#3
	}
	// #4
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&depthStencil)
		);

		device->CreateDepthStencilView(depthStencil.Get(), &depthStencilDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
		// !#4	// 创建深度模板视图
	}
	// #5
	{
		const uint32_t constantBufferDataSize = triangleResourceCount * sizeof(SceneConstantBuffer);

		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constantBufferDataSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBuffer));

		for (uint32_t n = 0; n < triangleCount; n++)
		{
			constantBufferData[n].velocity = XMFLOAT4(Random(0.01f, 0.02f), 0.0f, 0.0f, 0.0f);
			constantBufferData[n].offset = XMFLOAT4(Random(-5.0f, -1.5f), Random(-1.0f, 1.0f), Random(0.0f, 2.0f), 0.0f);
			constantBufferData[n].color = XMFLOAT4(Random(0.5f, 1.0f), Random(0.5f, 1.0f), Random(0.5f, 1.0f), 1.0f);
			XMStoreFloat4x4(&constantBufferData[n].projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.01f, 20.0f)));
		}	// 为每一个三角形初始化常量缓冲

		CD3DX12_RANGE readRange1(0, 0);
		constantBuffer->Map(0, &readRange1, reinterpret_cast<void**>(&pCbvDataBegin));
		memcpy(pCbvDataBegin, &constantBufferData[0], triangleCount * sizeof(SceneConstantBuffer));
		// 映射和初始化常量缓冲。应用程序关闭前不会取消映射。

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc1 = {};
		srvDesc1.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc1.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc1.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc1.Buffer.NumElements = triangleCount;
		srvDesc1.Buffer.StructureByteStride = sizeof(SceneConstantBuffer);
		srvDesc1.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		// 创建常量缓冲区的着色器资源视图（SRV），以供计算着色器读取。

		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvOffset, cbvSrvUavDescriptorSize);
		for (uint32_t frame = 0; frame < r_DefaultFrameCount_const; frame++)
		{
			srvDesc1.Buffer.FirstElement = frame * triangleCount;
			device->CreateShaderResourceView(constantBuffer.Get(), &srvDesc1, cbvSrvHandle);
			cbvSrvHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
		}
		// !#5
	}
	// #6
	{
		D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2] = {};
		argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
		argumentDescs[0].ConstantBufferView.RootParameterIndex = Cbv;
		argumentDescs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
		// 每个命令由CBV更新和DrawInstanced的调用组成

		D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
		commandSignatureDesc.pArgumentDescs = argumentDescs;
		commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
		commandSignatureDesc.ByteStride = sizeof(IndirectCommand);

		device->CreateCommandSignature(&commandSignatureDesc, rootSignature.Get(), IID_PPV_ARGS(&commandSignature));
		// !#6	// 创建用于间接绘图的命令签名。
	}
	// #7
	{
		std::vector<IndirectCommand> commands;
		commands.resize(triangleResourceCount);
		const uint32_t commandBufferSize = commandSizePerFrame * r_DefaultFrameCount_const;

		D3D12_RESOURCE_DESC commandBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(commandBufferSize);
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&commandBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&commandBuffer));

		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(commandBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&commandBufferUpload));

		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = constantBuffer->GetGPUVirtualAddress();
		uint32_t commandIndex = 0;

		for (uint32_t frame = 0; frame < r_DefaultFrameCount_const; frame++)
		{
			for (uint32_t n = 0; n < triangleCount; n++)
			{
				commands[commandIndex].cbv = gpuAddress;
				commands[commandIndex].drawArguments.VertexCountPerInstance = 3;
				commands[commandIndex].drawArguments.InstanceCount = 1;
				commands[commandIndex].drawArguments.StartVertexLocation = 0;
				commands[commandIndex].drawArguments.StartInstanceLocation = 0;

				commandIndex++;
				gpuAddress += sizeof(SceneConstantBuffer);
			}
		}

		D3D12_SUBRESOURCE_DATA commandData = {};
		commandData.pData = reinterpret_cast<uint8_t*>(&commands[0]);
		commandData.RowPitch = commandBufferSize;
		commandData.SlicePitch = commandData.RowPitch;
		// 将数据复制到中间上传堆，然后将上传堆的副本拷贝到命令缓冲区。

		UpdateSubresources<1>(commandList.Get(), commandBuffer.Get(), commandBufferUpload.Get(), 0, 0, 1, &commandData);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(commandBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2 = {};
		srvDesc2.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc2.Buffer.NumElements = triangleCount;
		srvDesc2.Buffer.StructureByteStride = sizeof(IndirectCommand);
		srvDesc2.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		// 为命令缓冲区创建SRV。

		CD3DX12_CPU_DESCRIPTOR_HANDLE commandsHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), CommandsOffset, cbvSrvUavDescriptorSize);
		for (uint32_t frameI = 0; frameI < r_DefaultFrameCount_const; frameI++)
		{
			srvDesc2.Buffer.FirstElement = frameI * triangleCount;
			device->CreateShaderResourceView(commandBuffer.Get(), &srvDesc2, commandsHandle);
			commandsHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE processedCommandsHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), ProcessedCommandsOffset, cbvSrvUavDescriptorSize);
		// 创建存储计算工作结果的无序访问视图（UAV）。
		for (uint32_t frame = 0; frame < r_DefaultFrameCount_const; frame++)
		{
			commandBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(commandBufferCounterOffset + sizeof(uint32_t), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&commandBufferDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&processedCommandBuffers[frame]));
			// 分配足够大的缓冲区以容纳单个帧的所有间接命令以及UAV计数器。

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = triangleCount;
			uavDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
			uavDesc.Buffer.CounterOffsetInBytes = commandBufferCounterOffset;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

			device->CreateUnorderedAccessView(
				processedCommandBuffers[frame].Get(),
				processedCommandBuffers[frame].Get(),
				&uavDesc,
				processedCommandsHandle);

			processedCommandsHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
		}

		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint32_t)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&processedCommandBufferCounterReset));
		// 分配一个可用于重置UAV计数器并将其初始化为0的缓冲区。

		uint8_t* pMappedCounterReset = nullptr;
		CD3DX12_RANGE readRange2(0, 0);
		processedCommandBufferCounterReset->Map(0, &readRange2, reinterpret_cast<void**>(&pMappedCounterReset));
		ZeroMemory(pMappedCounterReset, sizeof(uint32_t));
		processedCommandBufferCounterReset->Unmap(0, nullptr);
		// !#7	//创建命令缓冲区和UAV以存储计算工作的结果。
	}
	// #8
	{
		commandList->Close();
		ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		// !#8
	}
	// #9
	{
		device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&computeFence));
		fenceValues[frameIndex]++;

		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent == nullptr)
		{
			HRESULT_FROM_WIN32(GetLastError());
		}
		WaitForGpu();
		// !#9
	}
}

void ExecuteIndirect::PopulateCommandList()
{
	computeCommandAllocators[frameIndex]->Reset();
	commandAllocators[frameIndex]->Reset();

	computeCommandList->Reset(computeCommandAllocators[frameIndex].Get(), computeState.Get());
	commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get());

	if (enableCulling)
	{
		uint32_t frameDescriptorOffset = frameIndex * CbvSrvUavDescriptorCountPerFrame;
		D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvUavHandle = cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();

		computeCommandList->SetComputeRootSignature(computeRootSignature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = { cbvSrvUavHeap.Get() };
		computeCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		computeCommandList->SetComputeRootDescriptorTable(
			SrvUavTable,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvUavHandle, CbvSrvOffset + frameDescriptorOffset, cbvSrvUavDescriptorSize));

		computeCommandList->SetComputeRoot32BitConstants(RootConstants, 4, reinterpret_cast<void*>(&csRootConstants), 0);

		computeCommandList->CopyBufferRegion(processedCommandBuffers[frameIndex].Get(), commandBufferCounterOffset, processedCommandBufferCounterReset.Get(), 0, sizeof(uint32_t));
		// 为这一帧重置UAV计数器

		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(processedCommandBuffers[frameIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		computeCommandList->ResourceBarrier(1, &barrier);

		computeCommandList->Dispatch(static_cast<uint32_t>(ceil(triangleCount / float(computeThreadBlockSize))), 1, 1);
	}
	computeCommandList->Close();

	commandList->SetGraphicsRootSignature(rootSignature.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { cbvSrvUavHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, enableCulling ? &cullingScissorRect : &scissorRect);

	D3D12_RESOURCE_BARRIER barriers[2] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
			enableCulling ? processedCommandBuffers[frameIndex].Get() : commandBuffer.Get(),
			enableCulling ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT),
		CD3DX12_RESOURCE_BARRIER::Transition(
			renderTargets[frameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET)
	};
	// 指示命令缓冲区将用于间接绘制，并且后缓冲区将用作渲染目标。

	commandList->ResourceBarrier(_countof(barriers), barriers);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	if (enableCulling)
	{
		//PIXBeginEvent(commandList.Get(), 0, L"Draw visible triangles");

		commandList->ExecuteIndirect(
			commandSignature.Get(),
			triangleCount,
			processedCommandBuffers[frameIndex].Get(),
			0,
			processedCommandBuffers[frameIndex].Get(),
			commandBufferCounterOffset);
		// 画没有被遮挡的三角形
	}
	else
	{
		//PIXBeginEvent(commandList.Get(), 0, L"Draw all triangles");

		commandList->ExecuteIndirect(
			commandSignature.Get(),
			triangleCount,
			commandBuffer.Get(),
			commandSizePerFrame * frameIndex,
			nullptr,
			0);
		// 画所有的三角形
	}
	//PIXEndEvent(commandList.Get());

	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
	barriers[0].Transition.StateAfter = enableCulling ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// 指示计算着色器可以使用命令缓冲区，并且现在将使用后缓冲区来渲染。

	commandList->ResourceBarrier(_countof(barriers), barriers);

	commandList->Close();
}

void ExecuteIndirect::WaitForGpu()
{
	commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
	// 在队列中调度信号命令。

	fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
	WaitForSingleObjectEx(fenceEvent, INFINITE, false);

	fenceValues[frameIndex]++;
}

void ExecuteIndirect::MoveToNextFrame()
{
	const uint64_t currentFenceValue = fenceValues[frameIndex];
	commandQueue->Signal(fence.Get(), currentFenceValue);
	// 在队列中调度信号命令。

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	// 更新帧编号

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		WaitForSingleObjectEx(fenceEvent, INFINITE, false);
	}// 如果下一帧还没有渲染完，则等待

	fenceValues[frameIndex] = static_cast<uint32_t>(currentFenceValue) + 1;
}

void ExecuteIndirect::WaitForRenderContext()
{
}

uint32_t ExecuteIndirect::AlignForUavCounter(uint32_t bufferSize)
{
	const uint32_t alignment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
	return (bufferSize + (alignment - 1)) & ~(alignment - 1);
}	// ???