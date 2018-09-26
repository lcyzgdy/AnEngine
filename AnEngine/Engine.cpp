#include "Engine.h"
#include "RenderCore.h"
#include "Screen.h"
#include "Input.h"
#include "ThreadPool.hpp"
#include "DTimer.h"
#include "SceneManager.h"

namespace AnEngine
{
	void Engine::BeforeUpdate()
	{
		//lock_guard<mutex> lock(m_mutex);
		DTimer::GetInstance()->Tick(nullptr);
		BaseInput::GetInstance()->Update();
		Utility::ThreadPool::Commit(bind(&Engine::OnUpdate, this));
	}

	void Engine::OnUpdate()
	{
		var activeScene = Game::SceneManager::ActiveScene();
		for (var obj : activeScene->GetAllGameObject())
		{
			if (!obj->Active()) continue;
			for (var be : obj->GetComponents())
			{
				be->BeforeUpdate();
			}
			for (var be : obj->GetComponents())
			{
				be->Update();
			}
			for (var be : obj->GetComponents())
			{
				be->AfterUpdate();
			}
		}
		Utility::ThreadPool::Commit(bind(&Engine::AfterUpdate, this));
	}

	void Engine::AfterUpdate()
	{
		RenderCore::R_Present();
		BaseInput::GetInstance()->ZeroInputState();
		if (m_running)
		{
			Utility::ThreadPool::Commit(bind(&Engine::BeforeUpdate, this));
		}
	}

	/*Engine* Engine::GetInstance()
	{
		static Engine driver;
		return &driver;
	}*/

	void Engine::Initialize(HWND hwnd, HINSTANCE hInstance, int screenw, int screenh)
	{
		if (m_initialized) return;
		m_initialized = true;
		BaseInput::GetInstance()->Initialize(hwnd, hInstance);
		Screen::GetInstance()->Initialize(screenw, screenh);
		RenderCore::InitializeRender(hwnd);
	}

	void Engine::Release()
	{
		m_initialized = false;
		m_running = false;
		EndBehaviour();
		BaseInput::GetInstance()->Release();
	}

	void Engine::StartScene()
	{
		m_running = true;
		Utility::ThreadPool::Commit(bind(&Engine::BeforeUpdate, this));
	}

	void Engine::EndBehaviour()
	{
		//m_future.wait();
		//m_scene->OnRelease();
	}
}
