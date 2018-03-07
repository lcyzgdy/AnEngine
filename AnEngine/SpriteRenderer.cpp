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

	void SpriteRenderer::OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator)
	{
		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, m_pso->GetPSO()));
		
		iList->OMSetRenderTargets(1, &(m_renderTarget->GetRTV()), false, nullptr);
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