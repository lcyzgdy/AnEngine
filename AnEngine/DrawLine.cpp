#include "DrawLine.h"
using namespace std;
using namespace AnEngine::RenderCore;
using namespace Microsoft::WRL;

DrawLine::DrawLine(const HWND _hwnd, const UINT _width, const UINT _height) :
	D3D12AppBase(_hwnd, _width, _height),
	viewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height)),
	scissorRect(0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height)),
	rtvDescriptorSize(0), srvDescriptorSize(0),
	fenceValues{}, renderTargets{}
{
}

DrawLine::~DrawLine()
{
}

void DrawLine::OnInit()
{
	frameIndex = 0;
	rtvDescriptorSize = 0;

	InitializePipeline();
	InitializeAssets();

	//PostMessage(hwnd, WM_USER, 0, device->GetNodeCount());
}

void DrawLine::OnRelease()
{
	WaitForGpu();
	CloseHandle(fenceEvent);
}

void DrawLine::OnUpdate()
{
	if (BaseInput::GetInstance()->GetMouseButton(0))
	{
		Vertex v;
		//v.color = { 0.5f,0.1f,0.0f,1.0f };
		if (BaseInput::GetInstance()->GetMouseButtonDown(0))
		{
			v.color = { Random(0.0f,1.0f), Random(0.0f,1.0f),Random(0.0f,1.0f),1.0f };
			(*vertex.begin()).color = { Random(0.0f,1.0f), Random(0.0f,1.0f),Random(0.0f,1.0f),1.0f };
		}
		else v.color = (*vertex.rbegin()).color;
		auto pos = BaseInput::GetInstance()->GetM128MousePosition();

		v.position = XMFLOAT3(pos.m128_f32[0], pos.m128_f32[1], 0.0f);
		*(vertex.rbegin()) = v;

		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, vertex.data(), sizeof(Vertex)*vertex.size());
		vertexBuffer->Unmap(0, nullptr);
	}
}

void DrawLine::OnRender()
{
	PopulateCommandList();	//填入命令
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);	//提交命令
	swapChain->Present(1, 0);	//交换前后缓冲
	MoveToNextFrame();		//等待前一帧
}

void DrawLine::OnKeyDown(UINT8 key)
{
}

void DrawLine::OnKeyUp(UINT8 key)
{
}

void DrawLine::InitializePipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> d3dDebugController;
	if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
	{
		d3dDebugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	// 开启Debug模式

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	if (isUseWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	// 创设备

	//m_device->CheckFeatureSupport

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	// 描述并创建命令队列

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = r_DefaultFrameCount_const;
	swapChainDesc.Width = this->width;
	swapChainDesc.Height = this->height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	ComPtr<IDXGISwapChain1> swapChain1;
	// 描述并创建交换链
	ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(),		// 交换链需要队列，以便可以强制刷新它
		hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1));

	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
	swapChain1.As(&swapChain);
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	//#1
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = r_DefaultFrameCount_const;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	// 创建渲染目标视图描述符堆

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	// 描述并创建纹理的着色器资源视图堆 [Shader resource view (SRV) heap]

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// !#1	创建描述符堆

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < r_DefaultFrameCount_const; i++)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);

		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));
		//// !!!!
	}
	// 创建命令分配器
	// 创建帧资源
}

void DrawLine::InitializeAssets()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//Create an empty root signature.

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	//auto l = GetAssetFullPath(_T("shaders.hlsl"));
	D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
	D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	//psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, FALSE, TRUE, TRUE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	//Describe and create the graphics pipeline state object (PSO).

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList));
	//Command lists are created in the recording state, but there is nothing
	//Create the command list.

	//To record yet. The main loop expects it to be closed, so close it now.
	commandList->Close();
	//Close the command list
	vertex.push_back({ { 0.0f, 0.0f, 0.0f }, { Random(0.0f,1.0f), Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f} });
	vertex.push_back({ { 0.0f, 0.3f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } });

	const UINT vertexBufferSize = sizeof(Vertex) * 1000;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)));

	// 将数据复制到顶点缓存里
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, vertex.data(), sizeof(Vertex) * vertex.size());
	vertexBuffer->Unmap(0, nullptr);

	//Initialize the vertex buffer view.
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	//Create a m_fence

	fenceValues[frameIndex]++;
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		HRESULT_FROM_WIN32(GetLastError());
	}
	WaitForGpu();
}

void DrawLine::WaitForGpu()
{
	commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
	// 在队列中调度信号命令。

	fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
	WaitForSingleObjectEx(fenceEvent, INFINITE, false);

	fenceValues[frameIndex]++;
}

void DrawLine::PopulateCommandList()
{
	commandAllocators[frameIndex]->Reset();
	// 命令列表分配器只能在关联的命令列表已在GPU上完成执行时重置; 
	// 应用程序应该使用栅栏来确定GPU执行进度。

	commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get());
	// 但是，当在特定命令列表上调用ExecuteCommandList（）时，该命令列表随后可以在任何时间重置，
	// 但要在重新记录之前。

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	// 设置必要的状态。

	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	commandList->DrawInstanced(static_cast<UINT>(vertex.size()), 1, 0, 0);
	/*
	UpdateSubresources<1>(commandList.Get(), vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	*/
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	// 指示现在将使用后缓冲来呈现

	commandList->Close();
}

void DrawLine::MoveToNextFrame()
{
	const UINT64 currentFenceValue = fenceValues[frameIndex];
	commandQueue->Signal(fence.Get(), currentFenceValue);
	// 在队列中调度信号命令。

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	// 更新帧编号

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		WaitForSingleObjectEx(fenceEvent, INFINITE, false);
	}// 如果下一帧还没有渲染完，则等待

	fenceValues[frameIndex] = static_cast<UINT>(currentFenceValue) + 1;
}

void DrawLine::WaitForRenderContext()
{
}

DrawLineWithWu::DrawLineWithWu(const HWND _hwnd, const UINT _width, const UINT _height) :
	D3D12AppBase(_hwnd, _width, _height),
	viewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height)),
	scissorRect(0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height)),
	rtvDescriptorSize(0), srvDescriptorSize(0),
	fenceValues{}, renderTargets{}
{
}

DrawLineWithWu::~DrawLineWithWu()
{
}

void DrawLineWithWu::OnInit()
{
	frameIndex = 0;
	rtvDescriptorSize = 0;

	InitializePipeline();
	InitializeAssets();
}

void DrawLineWithWu::OnRelease()
{
	WaitForGpu();
	CloseHandle(fenceEvent);
}

void DrawLineWithWu::OnUpdate()
{
	if (BaseInput::GetInstance()->GetMouseButton(0))
	{
		Vertex v;
		//v.color = { 0.5f,0.1f,0.0f,1.0f };
		if (BaseInput::GetInstance()->GetMouseButtonDown(0))
		{
			v.color = { Random(0.0f,1.0f), Random(0.0f,1.0f),Random(0.0f,1.0f),1.0f };
			(*vertex.begin()).color = { Random(0.0f,1.0f), Random(0.0f,1.0f),Random(0.0f,1.0f),1.0f };
		}
		else v.color = (*vertex.rbegin()).color;
		auto pos = BaseInput::GetInstance()->GetM128MousePosition();

		v.position = XMFLOAT3(pos.m128_f32[0], pos.m128_f32[1], 0.0f);
		*(vertex.rbegin()) = v;

		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, vertex.data(), sizeof(Vertex)*vertex.size());
		vertexBuffer->Unmap(0, nullptr);
	}
}

void DrawLineWithWu::OnRender()
{
	PopulateCommandList();	//填入命令
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);	//提交命令
	swapChain->Present(1, 0);	//交换前后缓冲
	MoveToNextFrame();		//等待前一帧
}

void DrawLineWithWu::OnKeyDown(UINT8 key)
{
}

void DrawLineWithWu::OnKeyUp(UINT8 key)
{
}

void DrawLineWithWu::InitializePipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> d3dDebugController;
	if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
	{
		d3dDebugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	// 开启Debug模式

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	if (isUseWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	// 创设备

	//m_device->CheckFeatureSupport

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	// 描述并创建命令队列

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

	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
	swapChain1.As(&swapChain);
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	//#1
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = r_DefaultFrameCount_const;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	// 创建渲染目标视图描述符堆

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	// 描述并创建纹理的着色器资源视图堆 [Shader resource view (SRV) heap]

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// !#1	创建描述符堆

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < r_DefaultFrameCount_const; i++)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);

		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));
		//// !!!!
	}
	// 创建命令分配器
	// 创建帧资源
}

void DrawLineWithWu::InitializeAssets()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//Create an empty root signature.

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	//auto l = GetAssetFullPath(_T("shaders.hlsl"));
	D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
	D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	//psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, FALSE, TRUE, TRUE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	//Describe and create the graphics pipeline state object (PSO).

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList));
	//Command lists are created in the recording state, but there is nothing
	//Create the command list.

	//To record yet. The main loop expects it to be closed, so close it now.
	commandList->Close();
	//Close the command list
	start = { { 0.0f, 0.0f, 0.0f },{ Random(0.0f,1.0f), Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } };
	end = { { 0.0f, 0.3f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } };

	const UINT vertexBufferSize = sizeof(Vertex) * 1000;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)));

	// 将数据复制到顶点缓存里
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, vertex.data(), sizeof(Vertex) * vertex.size());
	vertexBuffer->Unmap(0, nullptr);

	//Initialize the vertex buffer view.
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	//Create a m_fence

	fenceValues[frameIndex]++;
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		HRESULT_FROM_WIN32(GetLastError());
	}
	WaitForGpu();
}

void DrawLineWithWu::WaitForGpu()
{
	commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
	// 在队列中调度信号命令。

	fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
	WaitForSingleObjectEx(fenceEvent, INFINITE, false);

	fenceValues[frameIndex]++;
}

void DrawLineWithWu::PopulateCommandList()
{
	commandAllocators[frameIndex]->Reset();
	// 命令列表分配器只能在关联的命令列表已在GPU上完成执行时重置; 
	// 应用程序应该使用栅栏来确定GPU执行进度。

	commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get());
	// 但是，当在特定命令列表上调用ExecuteCommandList（）时，该命令列表随后可以在任何时间重置，
	// 但要在重新记录之前。

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	// 设置必要的状态。

	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	commandList->DrawInstanced(static_cast<UINT>(vertex.size()), 1, 0, 0);
	/*
	UpdateSubresources<1>(commandList.Get(), vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	*/
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	// 指示现在将使用后缓冲来呈现

	commandList->Close();
}

void DrawLineWithWu::MoveToNextFrame()
{
	const UINT64 currentFenceValue = fenceValues[frameIndex];
	commandQueue->Signal(fence.Get(), currentFenceValue);
	// 在队列中调度信号命令。

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	// 更新帧编号

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		WaitForSingleObjectEx(fenceEvent, INFINITE, false);
	}// 如果下一帧还没有渲染完，则等待

	fenceValues[frameIndex] = static_cast<UINT>(currentFenceValue) + 1;
}

void DrawLineWithWu::WaitForRenderContext()
{
}

float DrawTriangle::Cross(Vertex & a, Vertex & b)
{
	return (a.position.x * b.position.y - b.position.x * a.position.y);
}

float DrawTriangle::Cross(Vertex& a, Vertex& b, Vertex& c)
{
	return ((a.position.x - c.position.x)*(b.position.y - c.position.y) - (b.position.x - c.position.x)*(a.position.y - c.position.y));
}

void DrawTriangle::Graham()
{
	while (!vertexIndexStack.empty()) vertexIndexStack.pop();
	if (vertex.size() <= 2) return;
	Vertex p1, p2;
	sort(vertex.begin(), vertex.end(), [](Vertex& a, Vertex& b)->
		bool
	{
		if (a.position.y == b.position.y) return a.position.x < b.position.x;
		return a.position.y < b.position.y;
	});
	vertexIndexStack.push(0);
	vertexIndexStack.push(1);
	int a, b;
	for (int i = 2; i < vertex.size(); i++)
	{
		while (1)
		{
			if (vertexIndexStack.size() < 2) break;
			a = vertexIndexStack.top();
			vertexIndexStack.pop();
			b = vertexIndexStack.top();
			p1.position.x = vertex[a].position.x - vertex[b].position.x;
			p1.position.y = vertex[a].position.y - vertex[b].position.y;
			p2.position.x = vertex[i].position.x - vertex[a].position.x;
			p2.position.y = vertex[i].position.y - vertex[a].position.y;
			if (Cross(p1, p2) < 0.0f)
			{
				vertexIndexStack.push(a);//包括共线点，若为“<”，则不包括。   
				break;
			}
		}
		vertexIndexStack.push(i);
	}
	int w = static_cast<int>(vertexIndexStack.size());
	//vertexIndexStack.push(vertex.size() - 2);
	if (vertex.size() - 2 <= 0) return;
	for (size_t i = vertex.size() - 2; i > 0; i--)
	{
		while (1)
		{
			if (vertexIndexStack.size() == w) break;
			a = vertexIndexStack.top();
			vertexIndexStack.pop();
			b = vertexIndexStack.top();
			p1.position.x = vertex[a].position.x - vertex[b].position.x;
			p1.position.y = vertex[a].position.y - vertex[b].position.y;
			p2.position.x = vertex[i].position.x - vertex[a].position.x;
			p2.position.y = vertex[i].position.y - vertex[a].position.y;
			if (Cross(p1, p2) < 0.0f)
			{
				vertexIndexStack.push(a);//包括共线点，若为“<”，则不包括   
				break;
			}
		}
		vertexIndexStack.push(static_cast<int>(i));
	}
}

DrawTriangle::DrawTriangle(const HWND _hwnd, const UINT _width, const UINT _height) :
	D3D12AppBase(_hwnd, _width, _height),
	viewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height)),
	scissorRect(0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height)),
	rtvDescriptorSize(0), srvDescriptorSize(0),
	fenceValues{}, renderTargets{}
{
}

DrawTriangle::~DrawTriangle()
{
}

void DrawTriangle::OnInit()
{
	frameIndex = 0;
	rtvDescriptorSize = 0;

	InitializePipeline();
	InitializeAssets();
}

void DrawTriangle::OnRelease()
{
	WaitForGpu();
	CloseHandle(fenceEvent);
}

void DrawTriangle::OnUpdate()
{
	if (BaseInput::GetInstance()->GetMouseButtonDown(0))
	{
		Vertex v;
		v.color = { Random(0.0f,1.0f), Random(0.0f,1.0f) ,Random(0.0f,1.0f) ,1.0f };
		auto pos = BaseInput::GetInstance()->GetM128MousePosition();

		v.position = XMFLOAT3(pos.m128_f32[0], pos.m128_f32[1], 0.0f);
		vertex.push_back(v);

		Graham();

		vector<Vertex> newVertex;
		while (!vertexIndexStack.empty())
		{
			newVertex.push_back(vertex[vertexIndexStack.top()]);
			vertexIndexStack.pop();
		}
		reverse(newVertex.begin(), newVertex.end());
		//if (newVertex.size() > 0) newVertex.push_back(newVertex[0]);

		vector<Vertex> finalVertex;
		if (newVertex.size() >= 3)
		{
			for (int i = 1; i < newVertex.size() - 1; i++)
			{
				finalVertex.push_back(newVertex[i]);
				finalVertex.push_back(newVertex[i + 1]);
				finalVertex.push_back(newVertex[0]);
			}
		}

		ofstream out("log.txt", ios::app);
		for (int i = 0; i < newVertex.size(); i++)
		{
			out << newVertex[i].position.x << ' ' << newVertex[i].position.y << endl;
		}
		out << '#' << endl;
		for (int i = 0; i < finalVertex.size(); i++)
		{
			out << finalVertex[i].position.x << ' ' << finalVertex[i].position.y << endl;
		}
		out << '@' << endl << endl;

		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, finalVertex.data(), sizeof(Vertex) * finalVertex.size());
		vertexBuffer->Unmap(0, nullptr);
	}
}

void DrawTriangle::OnRender()
{
	PopulateCommandList();	//填入命令
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);	//提交命令
	swapChain->Present(1, 0);	//交换前后缓冲
	MoveToNextFrame();		//等待前一帧
}

void DrawTriangle::OnKeyDown(UINT8 key)
{
}

void DrawTriangle::OnKeyUp(UINT8 key)
{
}

void DrawTriangle::InitializePipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> d3dDebugController;
	if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
	{
		d3dDebugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	// 开启Debug模式

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	if (isUseWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}
	// 创设备

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	// 描述并创建命令队列

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

	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
	swapChain1.As(&swapChain);
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	//#1
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = r_DefaultFrameCount_const;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	// 创建渲染目标视图描述符堆

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	// 描述并创建纹理的着色器资源视图堆 [Shader resource view (SRV) heap]

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// !#1	创建描述符堆

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < r_DefaultFrameCount_const; i++)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);

		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));
		//// !!!!
	}
	// 创建命令分配器
	// 创建帧资源
}

void DrawTriangle::InitializeAssets()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//Create an empty root signature.

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	//auto l = GetAssetFullPath(_T("shaders.hlsl"));
	D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
	D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	//psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, FALSE, TRUE, TRUE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	//Describe and create the graphics pipeline state object (PSO).

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList));
	//Command lists are created in the recording state, but there is nothing
	//Create the command list.

	//To record yet. The main loop expects it to be closed, so close it now.
	commandList->Close();
	//Close the command list
	//vertex.push_back({ { 0.0f, 0.0f, 0.0f },{ random(0.0f,1.0f), random(0.0f,1.0f), random(0.0f,1.0f), 1.0f } });
	//vertex.push_back({ { 0.0f, 0.3f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } });


	const UINT vertexBufferSize = sizeof(Vertex) * 1000;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)));

	// 将数据复制到顶点缓存里
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, vertex.data(), sizeof(Vertex) * vertex.size());
	vertexBuffer->Unmap(0, nullptr);

	//Initialize the vertex buffer view.
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	//Create a m_fence

	fenceValues[frameIndex]++;
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		HRESULT_FROM_WIN32(GetLastError());
	}
	WaitForGpu();
}

void DrawTriangle::WaitForGpu()
{
	commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
	// 在队列中调度信号命令。

	fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
	WaitForSingleObjectEx(fenceEvent, INFINITE, false);

	fenceValues[frameIndex]++;
}

void DrawTriangle::PopulateCommandList()
{
	commandAllocators[frameIndex]->Reset();
	// 命令列表分配器只能在关联的命令列表已在GPU上完成执行时重置; 
	// 应用程序应该使用栅栏来确定GPU执行进度。

	commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get());
	// 但是，当在特定命令列表上调用ExecuteCommandList（）时，该命令列表随后可以在任何时间重置，
	// 但要在重新记录之前。

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	// 设置必要的状态。

	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	commandList->DrawInstanced(static_cast<UINT>(vertex.size()), 1, 0, 0);
	/*
	UpdateSubresources<1>(commandList.Get(), vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	*/
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	// 指示现在将使用后缓冲来呈现

	commandList->Close();
}

void DrawTriangle::MoveToNextFrame()
{
	const UINT64 currentFenceValue = fenceValues[frameIndex];
	commandQueue->Signal(fence.Get(), currentFenceValue);
	// 在队列中调度信号命令。

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	// 更新帧编号

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		WaitForSingleObjectEx(fenceEvent, INFINITE, false);
	}// 如果下一帧还没有渲染完，则等待

	fenceValues[frameIndex] = static_cast<UINT>(currentFenceValue) + 1;
}

void DrawTriangle::WaitForRenderContext()
{
}
