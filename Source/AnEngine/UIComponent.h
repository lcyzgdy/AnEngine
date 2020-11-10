#pragma once
#ifndef __UICOMPONENT_H__
#define __UICOMPONENT_H__

#include "Renderer.h"
#include "GraphicsCard2D.h"

namespace AnEngine::Game::UI
{
	class UIComponent : public Renderer
	{
	public:
		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender(ID3D12GraphicsCommandList * iList, ID3D12CommandAllocator * iAllocator) final;

		virtual void OnGUI() = 0;
	};
}

#endif // !__UICOMPONENT_H__
