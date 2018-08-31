#include "DrawCube.h"
using namespace AnEngine::RenderCore;
using namespace Microsoft::WRL;

DrawCube::DrawCube(const HWND _hwnd, const uint32_t _width, const uint32_t _height) :
	D3D12AppBase(_hwnd, _width, _height),
	frameIndex(0),
	viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
	scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
	rtvDescriptorSize(0), cbvSrvUavDescriptorSize(0), dsvDescriptorSize(0), samDescriptorSize(0),
	fenceValues{},
	renderTargets{}
{
}

void DrawCube::OnInit()
{
	camera.OnInit({ 0.0f, 0.0f, 5.0f });
	camera.SetMoveSpeed(1.0f);

	projectionMatrix = camera.GetProjectionMatrix(0.8f, aspectRatio);

	InitializePipeline();
	InitializeAssets();
}

void DrawCube::OnRelease()
{
	WaitForGpu();

	CloseHandle(fenceEvent);
}

void DrawCube::OnUpdate()
{
	WaitForSingleObjectEx(swapChainEvent, 100, FALSE);

	DTimer::GetInstance()->Tick(NULL);
	camera.OnUpdate(static_cast<float>(DTimer::GetInstance()->GetElapsedSeconds()));
}

void DrawCube::OnRender()
{
	PopulateCommandList();

	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	ThrowIfFailed(swapChain->Present(1, 0));

	MoveToNextFrame();
}

void DrawCube::OnKeyDown(uint8_t key)
{
}

void DrawCube::OnKeyUp(uint8_t key)
{
}

void DrawCube::InitializePipeline()
{
	uint32_t dxgiFactoryFlags = 0;

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

	D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavDesc = {};
	cbvSrvUavDesc.NumDescriptors = 1;
	cbvSrvUavDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvSrvUavDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&cbvSrvUavDesc, IID_PPV_ARGS(&cbvSrvUavHeap));
	// 描述并创建纹理的着色器资源视图堆 [Shader resource view (SRV) heap]

	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&dsvHeap)));

	/*D3D12_DESCRIPTOR_HEAP_DESC samDesc = {};
	samDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	samDesc.NumDescriptors;
	samDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_device->CreateDescriptorHeap(&samDesc, IID_PPV_ARGS(&samHeap)));*/

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//samDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	// !#1	创建描述符堆

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (uint32_t i = 0; i < r_DefaultFrameCount_const; i++)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);

		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])));
	}
	// 创建命令分配器
	// 创建帧资源
}

void DrawCube::InitializeAssets()
{
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
		uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		uint32_t compileFlags = 0;
#endif
		//auto l = GetAssetFullPath(_T("shaders.hlsl"));
		D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
		D3DCompileFromFile(GetAssetFullPath(L"framebuffer_shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);


		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(XMFLOAT3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, FALSE, TRUE, TRUE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = true;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	}
	{
		ThrowIfFailed(DCompositionCreateDevice(nullptr, IID_PPV_ARGS(dCompositionDevice.GetAddressOf())));
		dCompositionDevice->CreateTargetForHwnd(hwnd, false, dCompositionTarget.GetAddressOf());
		dCompositionDevice->CreateVisual(dCompositionVisual.GetAddressOf());
		dCompositionVisual->SetContent(swapChain.Get());
		dCompositionTarget->SetRoot(dCompositionVisual.Get());
		dCompositionDevice->Commit();
	}

	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}
	}

	{
		VertexPositionColor cubeVertices[] = {
			{ { -1.0f, 1.0f, -1.0f},{ Random(0.0f,1.0f),Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } },	// Back Top Left
			{ { 1.0f, 1.0f, -1.0f},{ Random(0.0f,1.0f), Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } },	// Back Top Right
			{ { 1.0f, 1.0f, 1.0f },{ Random(0.0f,1.0f), Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } },	// Front Top Right
			{ { -1.0f, 1.0f, 1.0f},{ Random(0.0f,1.0f), Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } },	// Front Top Left
			{ { -1.0f, -1.0f, -1.0f },{ Random(0.0f,1.0f),Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } },	// Back Bottom Left
			{ { 1.0f, -1.0f, -1.0f},{ Random(0.0f,1.0f),Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } },	// Back Bottom Right
			{ { 1.0f, -1.0f, 1.0f },{ Random(0.0f,1.0f),Random(0.0f,1.0f), Random(0.0f,1.0f), 1.0f } },	// Front Bottom Right
			{ { -1.0f, -1.0f, 1.0f },{ Random(0.0f,1.0f) ,Random(0.0f,1.0f) ,Random(0.0f,1.0f), 1.0f } },	// Front Bottom Left
		};

		uint32_t cubeIndices[] =
		{
			0, 1, 3,
			1, 2, 3,

			3, 2, 7,
			6, 7, 2,

			2, 1, 6,
			5, 6, 1,

			1, 0, 5,
			4, 5, 0,

			0, 3, 4,
			7, 4, 3,

			7, 6, 4,
			5, 4, 6,
		};

		uint32_t vertexIndexBufferSize = sizeof(cubeIndices) + sizeof(cubeVertices);// +sizeof(quadVertices);

		ThrowIfFailed(device->CreateCommittedResource
		(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexIndexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexIndexBuffer)
		));
		uint8_t* mappedUploadHeap = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(vertexIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mappedUploadHeap)));

		memcpy(mappedUploadHeap, cubeVertices, sizeof(cubeVertices));
		memcpy(mappedUploadHeap + sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices));
		vertexIndexBuffer->Unmap(0, nullptr);

		cubeVbv.BufferLocation = vertexIndexBuffer.Get()->GetGPUVirtualAddress();
		cubeVbv.SizeInBytes = sizeof(cubeVertices);
		cubeVbv.StrideInBytes = sizeof(VertexPositionColor);
		cubeIbv.BufferLocation = cubeVbv.BufferLocation + sizeof(cubeVertices);
		cubeIbv.Format = DXGI_FORMAT_R16_UINT;
		cubeIbv.SizeInBytes = sizeof(cubeIndices);

		ThrowIfFailed(device->CreateCommandList
		(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocators[frameIndex].Get(),
			pipelineState.Get(),
			IID_PPV_ARGS(&commandList)
		));
		//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER));
	}
	commandList->Close();
	//ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	//commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	{
		ThrowIfFailed(device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		fenceValues[frameIndex]++;

		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
		WaitForGpu();
	}
}

void DrawCube::WaitForGpu()
{
	ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValues[frameIndex]));

	// Wait until the m_fence has been processed.
	ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	// Increment the m_fence value for the current frame.
	fenceValues[frameIndex]++;
}

void DrawCube::PopulateCommandList()
{
	ThrowIfFailed(commandAllocators[frameIndex]->Reset());

	ThrowIfFailed(commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get()));

	commandList->SetPipelineState(pipelineState.Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { cbvSrvUavHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	commandList->ClearRenderTargetView(rtvHandle, r_ClearColor_const_float, 0, nullptr);
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->SetGraphicsRootDescriptorTable(0, cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());
	commandList->IASetVertexBuffers(0, 1, &cubeVbv);
	commandList->IASetIndexBuffer(&cubeIbv);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(commandList->Close());
}

void DrawCube::MoveToNextFrame()
{
	const uint64_t currentFenceValue = fenceValues[frameIndex];
	ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue));

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	fenceValues[frameIndex] = static_cast<uint32_t>(currentFenceValue) + 1;
}

void DrawCube::WaitForRenderContext()
{
}
