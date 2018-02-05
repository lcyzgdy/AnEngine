#include "Renderer.h"
#include "CommandContext.h"
#include "RenderCore.h"
#include "GameObject.h"
#include "RootSignature.h"

namespace AnEngine::Game
{
	void Renderer::Start()
	{
		LoadAsset();
	}

	void Renderer::BeforeUpdate()
	{
		m_renderTarget = nullptr;
	}

	void Renderer::Update()
	{
		if (m_renderTarget != nullptr)
		{
			OnRender();
		}
	}

	void Renderer::AfterUpdate()
	{
	}

	void TrangleRender::LoadAsset()
	{
		var device = r_graphicsCard[0]->GetDevice();
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));

		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;
#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif
		D3DCompileFromFile(GetAssetFullPath(_T("framebuffer_shaders.hlsl")).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
		D3DCompileFromFile(GetAssetFullPath(_T("framebuffer_shaders.hlsl")).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);


		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		//psoDesc.pRootSignature = rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		//psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, FALSE,
			D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			FALSE, TRUE, TRUE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso)));


		Vertex triangleVertices[] =
		{
			//{ { -0.3f, 0.0f * aspectRatio, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
			{ { 0.0f, 0.3f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.3f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
			{ { 0.2f, -0.4f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }
		};
		const UINT vertexBufferSize = sizeof(Vertex) * 3;

		/*ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer)));*/

			//Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);		//We do not intend to read from this resource on the CPU.
		//vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		//memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		//vertexBuffer->Unmap(0, nullptr);

		//Initialize the vertex buffer view.
		//vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		//vertexBufferView.StrideInBytes = sizeof(Vertex);
		//vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	void TrangleRender::OnRender()
	{
		var commandList = GraphicsCommandContext::GetInstance()->GetOne();
		var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var pCommandList = commandList->GetCommandList();
		var pCommandAllocator = commandAllocator->GetAllocator();
		ThrowIfFailed(pCommandAllocator->Reset());
		pCommandList->Reset(pCommandAllocator, m_pso.GetPSO());

		//pCommandList->SetGraphicsRootSignature(m_pso.Get);
		//pCommandList->RSSetViewports(1, &viewport);
		//pCommandList->RSSetScissorRects(1, &scissorRect);
	}
}