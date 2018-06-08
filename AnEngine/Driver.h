#pragma once
#ifndef __DRIVER_H__
#define __DRIVER_H__

#include"onwind.h"
#include"BaseBehaviour.h"
#include"Scene.h"
#include"Camera.h"
#include"ObjectBehaviour.h"
#include"Renderer.h"
#include"Scene.h"
namespace AnEngine
{
	class Engine : public Object
	{
		bool m_initialized;
		bool m_running;
		Game::Scene* m_scene;
		std::mutex m_mutex;
		std::future<void> m_future;

		Engine() = default;
		virtual ~Engine() = default;

		void BeforeUpdate();
		void OnUpdate();
		void AfterUpdate();

	public:

		static Engine* GetInstance();

		void Initialize(HWND hwnd, HINSTANCE hInstance, int screenw, int screenh);
		void Release();

		void StartScene(Game::Scene* behaviour);
		void EndBehaviour();
	};
}

#endif