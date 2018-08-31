#include "Driver.h"
#include "RenderCore.h"
#include "Screen.h"
#include "Input.h"
#include "ThreadPool.hpp"
#include "DTimer.h"

namespace AnEngine
{
	void Engine::BeforeUpdate()
	{
		lock_guard<mutex> lock(m_mutex);
		BaseInput::GetInstance()->Update();
		DTimer::GetInstance()->Tick(nullptr);
		Utility::ThreadPool::Commit(bind(&Engine::OnUpdate, this));
	}

	void Engine::OnUpdate()
	{
		lock_guard<mutex> lock(m_mutex);
		m_scene->BeforeUpdate();
		m_scene->OnUpdate();
		m_scene->AfterUpdate();
		Utility::ThreadPool::Commit(bind(&Engine::AfterUpdate, this));
	}

	void Engine::AfterUpdate()
	{
		lock_guard<mutex> lock(m_mutex);
		RenderCore::R_Present();
		BaseInput::GetInstance()->ZeroInputState();
		if (m_running)
		{
			Utility::ThreadPool::Commit(bind(&Engine::BeforeUpdate, this));
		}
	}

	Engine* Engine::GetInstance()
	{
		static Engine driver;
		return &driver;
	}

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

	void Engine::StartScene(Game::Scene* behaviour)
	{
		m_scene = behaviour;
		behaviour->OnInit();
		m_running = true;
		Utility::ThreadPool::Commit(bind(&Engine::BeforeUpdate, this));
	}

	void Engine::EndBehaviour()
	{
		//m_future.wait();
		m_scene->OnRelease();
	}
}
