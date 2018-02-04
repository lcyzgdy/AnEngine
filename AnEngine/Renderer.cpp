#include "Renderer.h"
#include"CommandContext.h"
#include"RenderCore.h"
#include"GameObject.h"

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
	}

	void TrangleRender::OnRender()
	{
		var commandList = GraphicsCommandContext::GetInstance()->GetOne();
		var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var pCommandList = commandList->GetCommandList();
		var pCommandAllocator = commandAllocator->GetAllocator();
		ThrowIfFailed(pCommandAllocator->Reset());
		pCommandList->Reset(pCommandAllocator, m_pso.GetPSO());

		pCommandList->SetGraphicsRootSignature(m_pso.Get);
		pCommandList->RSSetViewports(1, &viewport);
		pCommandList->RSSetScissorRects(1, &scissorRect);
	}
}