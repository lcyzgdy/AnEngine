#include "DrawCube.h"

DrawCube::DrawCube(const HWND _hwnd, const UINT _width, const UINT _height) :
	D3D12AppBase(_hwnd, _width, _height), //m_psoLibrary(FrameCount, RootParameterUberShaderCB),
	frameIndex(0),
	swapChainEvent(0),
	//drawIndex(0),
	//maxDrawsPerFrame(256),
	dynamicCB(sizeof(DrawConstantBuffer), 256, DefaultFrameCount),
	viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
	scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
	rtvDescriptorSize(0),
	srvDescriptorSize(0),
	fenceValues{}
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

	timer.Tick(NULL);
	camera.OnUpdate(static_cast<float>(timer.GetElapsedSeconds()));
}

void DrawCube::OnRender()
{
	PIXBeginEvent(commandQueue.Get(), 0, L"Render");

	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	PIXEndEvent(commandQueue.Get());

	ThrowIfFailed(swapChain->Present(1, 0));

	MoveToNextFrame();
}

void DrawCube::OnKeyDown(UINT8 key)
{
}

void DrawCube::OnKeyUp(UINT8 key)
{
}

void DrawCube::InitializePipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	if (IsUseWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&device)
		));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&device)
		));
	}

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = DefaultFrameCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(
		commandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1
	));

	ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain1.As(&swapChain));
	frameIndex = swapChain->GetCurrentBackBufferIndex();
	swapChainEvent = swapChain->GetFrameLatencyWaitableObject();

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = DefaultFrameCount + 1;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));


		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create RTVs and a command allocator for each frame.
		for (UINT n = 0; n < DefaultFrameCount; n++)
		{
			ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
			device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);

			ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[n])));
		}

		D3D12_RESOURCE_DESC renderTargetDesc = renderTargets[0]->GetDesc();

		D3D12_CLEAR_VALUE clearValue = {};
		memcpy(clearValue.Color, ClearColor, sizeof(ClearColor));
		clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Create an intermediate render target that is the same dimensions as the swap chain.
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&renderTargetDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&clearValue,
			IID_PPV_ARGS(&intermediateRenderTarget)));


		device->CreateRenderTargetView(intermediateRenderTarget.Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);

		// Create a SRV of the intermediate render target.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = renderTargetDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());
		device->CreateShaderResourceView(intermediateRenderTarget.Get(), &srvDesc, srvHandle);
	}
}

void DrawCube::InitializeAssets()
{// Create the root signature.
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[RootParametersCount];
		ranges[RootParameterSRV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 rootParameters[RootParametersCount];
		rootParameters[RootParameterUberShaderCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[RootParameterCB].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[RootParameterSRV].InitAsDescriptorTable(1, &ranges[RootParameterSRV], D3D12_SHADER_VISIBILITY_PIXEL);

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = 9999.0f;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

	}

	// Create the command list.
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList)));

	ComPtr<ID3D12Resource> vertexIndexBufferUpload;

	// Vertex and Index Buffer.
	{
		const VertexPositionColor cubeVertices[] = {
			{ { -1.0f, 1.0f, -1.0f, 1.0f },{ random(0.0f,1.0f),random(0.0f,1.0f), random(0.0f,1.0f) } },	// Back Top Left
			{ { 1.0f, 1.0f, -1.0f, 1.0f },{ random(0.0f,1.0f), random(0.0f,1.0f), random(0.0f,1.0f) } },	// Back Top Right
			{ { 1.0f, 1.0f, 1.0f, 1.0f },{ random(0.0f,1.0f), random(0.0f,1.0f), random(0.0f,1.0f) } },	// Front Top Right
			{ { -1.0f, 1.0f, 1.0f, 1.0f },{ random(0.0f,1.0f), random(0.0f,1.0f), random(0.0f,1.0f) } },	// Front Top Left
			{ { -1.0f, -1.0f, -1.0f, 1.0f },{ random(0.0f,1.0f),random(0.0f,1.0f), random(0.0f,1.0f) } },	// Back Bottom Left
			{ { 1.0f, -1.0f, -1.0f, 1.0f },{ random(0.0f,1.0f),random(0.0f,1.0f), random(0.0f,1.0f) } },	// Back Bottom Right
			{ { 1.0f, -1.0f, 1.0f, 1.0f },{ random(0.0f,1.0f),random(0.0f,1.0f), random(0.0f,1.0f) } },	// Front Bottom Right
			{ { -1.0f, -1.0f, 1.0f, 1.0f },{ random(0.0f,1.0f) ,random(0.0f,1.0f) ,random(0.0f,1.0f) } },	// Front Bottom Left
		};

		const UINT cubeIndices[] =
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

		static const VertexPositionUV quadVertices[] =
		{
			{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },	// Bottom Left
			{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },	// Top Left
			{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },	// Bottom Right
			{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },		// Top Right
		};

		const UINT vertexIndexBufferSize = sizeof(cubeIndices) + sizeof(cubeVertices) + sizeof(quadVertices);

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexIndexBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&vertexIndexBuffer)));

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexIndexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexIndexBufferUpload)));


		UINT8* mappedUploadHeap = nullptr;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(vertexIndexBufferUpload->Map(0, &readRange, reinterpret_cast<void**>(&mappedUploadHeap)));

		// Fill in part of the upload heap with our index and vertex data.
		UINT8* heapLocation = static_cast<UINT8*>(mappedUploadHeap);
		memcpy(heapLocation, cubeVertices, sizeof(cubeVertices));
		heapLocation += sizeof(cubeVertices);
		memcpy(heapLocation, cubeIndices, sizeof(cubeIndices));
		heapLocation += sizeof(cubeIndices);
		memcpy(heapLocation, quadVertices, sizeof(quadVertices));

		// Pack the vertices and indices into their destination by copying from the upload heap.
		commandList->CopyBufferRegion(vertexIndexBuffer.Get(), 0, vertexIndexBufferUpload.Get(), 0, vertexIndexBufferSize);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER));

		// Create the index and vertex buffer views.
		cubeVbv.BufferLocation = vertexIndexBuffer.Get()->GetGPUVirtualAddress();
		cubeVbv.SizeInBytes = sizeof(cubeVertices);
		cubeVbv.StrideInBytes = sizeof(VertexPositionColor);

		cubeIbv.BufferLocation = cubeVbv.BufferLocation + sizeof(cubeVertices);
		cubeIbv.SizeInBytes = sizeof(cubeIndices);
		cubeIbv.Format = DXGI_FORMAT_R32_UINT;

		/*quadVbv.BufferLocation = cubeIbv.BufferLocation + sizeof(cubeIndices);
		quadVbv.SizeInBytes = sizeof(quadVertices);
		quadVbv.StrideInBytes = sizeof(VertexPositionUV);*/
	}

	// Create the constant buffer.
	dynamicCB.Init(device.Get());

	// Close the command list and execute it to begin the vertex/index buffer copy
	// into the default heap.
	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		ThrowIfFailed(device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		fenceValues[frameIndex]++;

		// Create an event handle to use for frame synchronization.
		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForGpu();
	}

	//m_psoLibrary.Build(device.Get(), m_rootSignature.Get());
}

void DrawCube::WaitForGpu()
{
	ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValues[frameIndex]));

	// Wait until the fence has been processed.
	ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	fenceValues[frameIndex]++;
}

void DrawCube::PopulateCommandList()
{
	ThrowIfFailed(commandAllocators[frameIndex]->Reset());
	ThrowIfFailed(commandList->Reset(commandAllocators[frameIndex].Get(), nullptr));

	commandList->SetGraphicsRootSignature(rootSignature.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE intermediateRtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), DefaultFrameCount, rtvDescriptorSize);
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(srvHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->OMSetRenderTargets(1, &intermediateRtvHandle, FALSE, nullptr);

	// Record commands.
	commandList->ClearRenderTargetView(intermediateRtvHandle, ClearColor, 0, nullptr);

	// Draw the scene as normal into the intermediate buffer.
	PIXBeginEvent(commandList.Get(), 0, L"Draw cube");
	{
		static float rot = 0.0f;
		DrawConstantBuffer* drawCB = (DrawConstantBuffer*)dynamicCB.GetMappedMemory(0, frameIndex);
		drawCB->worldViewProjection = XMMatrixTranspose(XMMatrixRotationY(rot) * XMMatrixRotationX(-rot) * camera.GetViewMatrix() * projectionMatrix);

		rot += 0.01f;

		commandList->IASetVertexBuffers(0, 1, &cubeVbv);
		commandList->IASetIndexBuffer(&cubeIbv);
		//psoLibrary.SetPipelineState(device.Get(), rootSignature.Get(), m_commandList.Get(), BaseNormal3DRender, m_frameIndex);

		//commandList->SetGraphicsRootConstantBufferView(RootParameterCB, dynamicCB.GetGpuVirtualAddress(drawIndex, m_frameIndex));
		commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
		//drawIndex++;
	}
	PIXEndEvent(commandList.Get());

	// Set up the state for a fullscreen quad.
	//commandList->IASetVertexBuffers(0, 1, &quadVbv);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Indicate that the back buffer will be used as a render target and the
	// intermediate render target will be used as a SRV.
	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
		CD3DX12_RESOURCE_BARRIER::Transition(intermediateRenderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	};

	commandList->ResourceBarrier(_countof(barriers), barriers);
	commandList->SetGraphicsRootDescriptorTable(RootParameterSRV, srvHeap->GetGPUDescriptorHandleForHeapStart());

	const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	commandList->ClearRenderTargetView(rtvHandle, black, 0, nullptr);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Draw some quads using the rendered scene with some effect shaders.
	PIXBeginEvent(commandList.Get(), 0, L"Post-processing");
	{
		UINT quadCount = 0;
		static const UINT quadsX = 3;
		static const UINT quadsY = 3;


		CD3DX12_VIEWPORT viewport(
			(quadCount % quadsX) * (viewport.Width / quadsX),
			(quadCount / quadsY) * (viewport.Height / quadsY),
			viewport.Width / quadsX,
			viewport.Height / quadsY);

		//PIXBeginEvent(commandList.Get(), 0, cEffectNames[i]);
		commandList->RSSetViewports(1, &viewport);
		//psoLibrary.SetPipelineState(device.Get(), rootSignature.Get(), commandList.Get(), static_cast<EffectPipelineType>(i), m_frameIndex);
		commandList->DrawInstanced(4, 1, 0, 0);
		PIXEndEvent(commandList.Get());
	}
	PIXEndEvent(commandList.Get());

	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	commandList->ResourceBarrier(_countof(barriers), barriers);

	ThrowIfFailed(commandList->Close());
}

void DrawCube::MoveToNextFrame()
{
	const UINT64 currentFenceValue = fenceValues[frameIndex];
	ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue));

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	fenceValues[frameIndex] = currentFenceValue + 1;
}

void DrawCube::WaitForRenderContext()
{
}
