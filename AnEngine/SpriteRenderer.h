#pragma once
#ifndef __SPRITERENDERER_H__
#define __SPRITERENDERER_H__

#include"Texture.h"
#include"Renderer.h"

namespace AnEngine::Game
{
	class SpriteRenderer : public Renderer
	{
		Resource::Texture* m_texture;
	public:
		explicit SpriteRenderer();
		explicit SpriteRenderer(Resource::Texture* tex);
		virtual ~SpriteRenderer() = default;

		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator) override;

		virtual void Destory() override;

		Resource::Texture* GetTexture();
		void SetTexture(Resource::Texture* newTex);
	};
}

#endif // !__SPRITERENDERER_H__
