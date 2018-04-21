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
	class Driver : public Object
	{
		bool m_initialized;
		Game::Scene* m_scene;

		Driver() = default;
		~Driver() = default;

	public:

		static Driver* GetInstance();

		void Initialize(HWND hwnd, HINSTANCE hInstance, int screenw, int screenh);
		void Release();

		void StartScene(Game::Scene* behaviour);
		void EndBehaviour();
	};
}

#endif