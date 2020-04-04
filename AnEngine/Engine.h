#pragma once
#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "onwind.h"
#include "Scene.h"
#include "ObjectBehaviour.h"
#include "Renderer.h"
#include "Scene.h"
#include "RenderPipeline.h"
#include "Win32App.h"
#include "GraphicsCard.h"

namespace AnEngine
{
	class DLL_API Engine : public Singleton<Engine>
	{
		friend class ::Singleton<Engine>;

		bool m_initialized;
		bool m_running;
		std::mutex m_sceneResMutex;
		std::mutex m_parallelMutex;

		RenderCore::BaseRenderPipeline* m_rp;

		Engine() = default;
		virtual ~Engine() = default;

		void UpdateBottom();
		void UpdateSystem();
		void UpdateBehaviour();

	public:
		void Initialize(const Win32App& win32App);
		void Release();

		const RenderCore::GraphicsCard& InitializeRender(IDXGIFactory6* dxgiFactory);
		void AttachSwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain4>& swapChain, uint32_t bufferCount);


		void StartScene();
		// void EndBehaviour();
	};
}

#endif