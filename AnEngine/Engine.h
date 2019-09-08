#pragma once
#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "onwind.h"
#include "BaseBehaviour.h"
#include "Scene.h"
#include "Camera.h"
#include "ObjectBehaviour.h"
#include "Renderer.h"
#include "Scene.h"
#include "RenderPipeline.h"

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

		void Initialize(HWND hwnd, HINSTANCE hInstance, int screenw, int screenh);
		void Release();

		void StartScene();
		void EndBehaviour();
	};
}

#endif