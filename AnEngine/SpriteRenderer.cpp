#include "SpriteRenderer.h"
#include "RenderCore.h"
#include "DTimer.h"
#include "CommandContext.h"
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::Game
{
	SpriteRenderer::SpriteRenderer(const wstring& name, Texture* tex) : Renderer(name)
	{
		m_texture = tex;
	}

	SpriteRenderer::SpriteRenderer(wstring&& name, Texture* tex) : Renderer(name)
	{
		m_texture = tex;
	}

	void SpriteRenderer::LoadAsset()
	{
	}

	void SpriteRenderer::OnRender()
	{
		m_renderTarget->GetFence()->CpuWait(Timer::GetTotalTicks());

		var commandList = GraphicsCommandContext::GetInstance()->GetOne();
		var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		iCommandList->OMSetRenderTargets(1, &(m_renderTarget->GetRTV()), false, nullptr);
		

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, m_pso->GetPSO()));
	}

	void SpriteRenderer::Destory()
	{
		Renderer::Destory();
		delete m_texture;
	}

	Texture* SpriteRenderer::GetTexture()
	{
		return m_texture;
	}

	void SpriteRenderer::SetTexture(Texture* newTex)
	{
		delete m_texture;
		m_texture = newTex;
	}
}