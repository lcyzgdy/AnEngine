#include "frameBuffering.h"

using namespace Microsoft::WRL;

FrameBuffering::FrameBuffering(const HWND _hwnd, const UINT _width, const UINT _height) :
	D3D12AppBase(_hwnd, _width, _height),
	viewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height)),
	scissorRect(0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height)),
	rtvDescriptorSize(0), srvDescriptorSize(0),
	fenceValues{}, renderTargets{}
{
}

FrameBuffering::~FrameBuffering()
{
}

void FrameBuffering::OnInit()
{
	frameIndex = 0;
	rtvDescriptorSize = 0;

	InitializePipeline();
	InitializeAssets();
}

void FrameBuffering::OnUpdate()
{
}

void FrameBuffering::OnRender()
{
	PopulateCommandList();	//填入命令
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);	//提交命令
	swapChain->Present(1, 0);	//交换前后缓冲
	MoveToNextFrame();		//等待前一帧
}

void FrameBuffering::OnRelease()
{
	WaitForGpu();
	CloseHandle(fenceEvent);
}

void FrameBuffering::OnKeyUp(UINT8 _key)
{
}

void FrameBuffering::OnKeyDown(UINT8 _key)
{
}

void FrameBuffering::InitializePipeline()
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

void FrameBuffering::InitializeAssets()
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

	Vertex triangleVertices[] =
	{
		//{ { -0.3f, 0.0f * aspectRatio, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.0f, 0.3f * aspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.3f, 0.0f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 0.2f, -0.4f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.2f, -0.4f * aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.3f, 0.0f * aspectRatio, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.1f, 0.3f * aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { -0.05f, 0.3f * aspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } }
	};
	const UINT vertexBufferSize = sizeof(Vertex) * 1000;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)));

	//Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);		//We do not intend to read from this resource on the CPU.
	vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
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
	//Create an event handle to use for frame synchronization.
	//Create synchronization objects.
	WaitForGpu();
}

void FrameBuffering::PopulateCommandList()
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

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	commandList->DrawInstanced(7, 1, 0, 0);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	// 指示现在将使用后缓冲来呈现

	commandList->Close();
}

void FrameBuffering::WaitForGpu()
{
	commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
	// 在队列中调度信号命令。

	fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
	WaitForSingleObjectEx(fenceEvent, INFINITE, false);

	fenceValues[frameIndex]++;
}

void FrameBuffering::MoveToNextFrame()
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

void FrameBuffering::WaitForRenderContext()
{
}
