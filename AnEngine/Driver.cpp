#include "Driver.h"
#include"RenderCore.h"
#include"Screen.h"
#include"Input.h"

namespace AnEngine
{
	Driver* Driver::GetInstance()
	{
		static Driver driver;
		return &driver;
	}

	void Driver::Initialize(HWND hwnd, HINSTANCE hInstance, int screenw, int screenh)
	{
		if (m_initialized) return;
		m_initialized = true;
		BaseInput::GetInstance()->Initialize(hwnd, hInstance);
		Screen::GetInstance()->Initialize(screenw, screenh);
		RenderCore::InitializeRender(hwnd);
	}

	void Driver::Release()
	{
		//RenderCore::rrrr_runningFlag = false;
		//this_thread::sleep_for(100ms);
		m_initialized = false;
		EndBehaviour();
		BaseInput::GetInstance()->Release();
	}

	void Driver::StartScene(Game::Scene* behaviour)
	{
		m_scene = behaviour;
		behaviour->OnInit();
	}

	void Driver::EndBehaviour()
	{
		m_scene->OnRelease();
	}
}
