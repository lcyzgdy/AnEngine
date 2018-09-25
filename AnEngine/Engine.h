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

namespace AnEngine
{
	class Engine : public ::Singleton<Engine>
	{
		friend class ::Singleton<Engine>;

		bool m_initialized;
		bool m_running;
		//std::mutex m_mutex;

		Engine() = default;
		virtual ~Engine() = default;

		void BeforeUpdate();
		void OnUpdate();
		void AfterUpdate();

	public:
		enum UpdateEvent
		{
		};

		enum RenderEvent
		{
			BeforeGBuffer,
			AfterGBuffer,
			AfterLighting,
			AfterShadow,
			AfterProcess
		};


		void Initialize(HWND hwnd, HINSTANCE hInstance, int screenw, int screenh);
		void Release();

		void StartScene(Game::Scene* behaviour);
		void EndBehaviour();
	};
}

#endif