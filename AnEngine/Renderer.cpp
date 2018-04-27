#include "Renderer.h"
#include "CommandContext.h"
#include "RenderCore.h"
#include "GameObject.h"
#include "RootSignature.h"
#include "Screen.h"
#include "Camera.h"
#include "DTimer.h"
using namespace std;
using namespace AnEngine::RenderCore;

namespace AnEngine::Game
{
	void Renderer::Start()
	{
		LoadAsset();
	}

	/*void Renderer::BeforeUpdate()
	{
		//m_renderTarget = nullptr;
	}*/

	void Renderer::Update()
	{
		m_renderTarget = Camera::FindForwordTarget(this->gameObject->transform.Position());
	}

	void Renderer::LateUpdate()
	{
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();
		var iAllocator = commandAllocator->GetAllocator();
		if (m_renderTarget != nullptr)
		{
			//m_renderTarget->GetFence()->CpuWait(0);
			OnRender(iList, iAllocator);
			//m_renderTarget->GetFence()->GpuSignal(0);
		}

		GraphicsContext::Push(commandList, commandAllocator);
	}

	Renderer::Renderer() : ObjectBehaviour()
	{
	}

	/*Renderer::Renderer(std::wstring&& name) : ObjectBehaviour(name)
	{
	}*/

	void Renderer::Destory()
	{
		ObjectBehaviour::Destory();
		delete m_pso;
		delete m_rootSignature;
		m_renderTarget = nullptr;
	}

	TrangleRender::TrangleRender() : Renderer(), m_viewport(0.0f, 0.0f,
		static_cast<float>(Screen::GetInstance()->Width()), static_cast<float>(Screen::GetInstance()->Height())),
		m_scissorRect(0, 0, static_cast<long>(Screen::GetInstance()->Width()),
			static_cast<long>(Screen::GetInstance()->Height()))
	{
	}

	/*TrangleRender::TrangleRender(std::wstring && name) : Renderer(name), m_viewport(0.0f, 0.0f,
		static_cast<float>(Screen::GetInstance()->Width()), static_cast<float>(Screen::GetInstance()->Height())),
		m_scissorRect(0, 0, static_cast<long>(Screen::GetInstance()->Width()),
			static_cast<long>(Screen::GetInstance()->Height()))
	{
	}*/

	void TrangleRender::LoadAsset()
	{
		var device = r_graphicsCard[0]->GetDevice();

		m_rootSignature = new RootSignature();

		/*ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;
#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif
		D3DCompileFromFile(GetAssetFullPath(_T("framebuffer_shaders.hlsl")).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
		D3DCompileFromFile(GetAssetFullPath(_T("framebuffer_shaders.hlsl")).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);
		*/
		m_vertexShader = new VertexShader(L"framebuffer_shaders.hlsl");
		m_pixelShader = new PixelShader(L"framebuffer_shaders.hlsl");


		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_BLEND_DESC blend;
		blend.AlphaToCoverageEnable = false;
		blend.IndependentBlendEnable = false;
		D3D12_RENDER_TARGET_BLEND_DESC rtb;
		rtb.BlendEnable = true;
		rtb.LogicOpEnable = false;
		rtb.SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
		rtb.DestBlend = D3D12_BLEND::D3D12_BLEND_ONE;
		rtb.BlendOp = D3D12_BLEND_OP_ADD;
		rtb.SrcBlendAlpha = D3D12_BLEND_ONE;
		rtb.DestBlendAlpha = D3D12_BLEND_ONE;
		rtb.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		rtb.LogicOp = D3D12_LOGIC_OP_NOOP;
		rtb.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blend.RenderTarget[0] = rtb;

		m_pso = new GraphicPSO();
		m_pso->SetInputLayout(_countof(inputElementDescs), inputElementDescs);
		m_pso->SetRootSignature(m_rootSignature->GetRootSignature());
		m_pso->SetVertexShader(m_vertexShader->GetByteCode());
		m_pso->SetPixelShader(m_pixelShader->GetByteCode());
		m_pso->SetRasterizerState(CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, false,
			D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			false, true, true, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF));
		//m_pso->SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
		m_pso->SetBlendState(blend);
		m_pso->SetDepthStencilState(false, false);
		//m_pso->SetSampleMask(UINT_MAX);
		m_pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
		m_pso->SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_pso->Finalize();


		Vertex triangleVertices[] =
		{
			{ { -0.3f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			{ { 0.0f, 0.66f, 0.0f }, { 1.0f, 1.0f, 1.0f, 0.0f } },
			{ { 0.3f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }
		};
		m_vertexBuffer = new Resource::ByteAddressBuffer(L"Trangle", 3, sizeof(Vertex), triangleVertices);

		//Initialize the vertex buffer view.
		//vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		//vertexBufferView.StrideInBytes = sizeof(Vertex);
		//vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	void TrangleRender::OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator)
	{
		//m_renderTarget->GetFence()->CpuWait(Timer::GetTotalTicks());
		ThrowIfFailed(iAllocator->Reset(), R_GetGpuError);
		ThrowIfFailed(iList->Reset(iAllocator, m_pso->GetPSO()), R_GetGpuError);

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToCommon;
		commonToRenderTarget.Transition.pResource = m_renderTarget->GetResource();
		renderTargetToCommon.Transition.pResource = m_renderTarget->GetResource();

		iList->ResourceBarrier(1, &commonToRenderTarget);
		iList->SetPipelineState(m_pso->GetPSO());
		iList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature());
		iList->RSSetViewports(1, &m_viewport);
		iList->RSSetScissorRects(1, &m_scissorRect);

		iList->OMSetRenderTargets(1, &(m_renderTarget->GetRTV()), false, nullptr);
		//iList->ClearRenderTargetView(m_renderTarget->GetRTV(), { 0.0f, 0.0f, 0, 1 }, 0, nullptr);
		//float color[4] = { 0, 0, 0, 1 };
		//iList->ClearRenderTargetView(m_renderTarget->GetRTV(), color, 0, nullptr);
		iList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		iList->IASetVertexBuffers(0, 1, &(m_vertexBuffer->VertexBufferView()));
		iList->DrawInstanced(3, 1, 0, 0);

		iList->ResourceBarrier(1, &renderTargetToCommon);

		ThrowIfFailed(iList->Close());

		//ID3D12CommandList* ppcommandList[] = { iList };
		//r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);

		//m_renderTarget->GetFence()->GpuSignal(Timer::GetTotalTicks());
	}

	void TrangleRender::Destory()
	{
		delete m_vertexBuffer;
		delete m_vertexShader;
		delete m_pixelShader;
		delete m_rootSignature;
		delete m_pso;
	}
}