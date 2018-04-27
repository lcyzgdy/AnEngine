#pragma once
#ifndef __SPRITERENDERER_H__
#define __SPRITERENDERER_H__

#include"Texture.h"
#include"Renderer.h"

namespace AnEngine::Game
{
	class SpriteRenderer : public Renderer
	{
		RenderCore::Resource::Texture* m_texture;
	public:
		explicit SpriteRenderer();
		explicit SpriteRenderer(RenderCore::Resource::Texture* tex);
		virtual ~SpriteRenderer() = default;

		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator) override;

		virtual void Destory() override;

		RenderCore::Resource::Texture* GetTexture();
		void SetTexture(RenderCore::Resource::Texture* newTex);
	};
}

#endif // !__SPRITERENDERER_H__
