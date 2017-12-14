#include "Driver.h"
#include"RenderCore.h"
#include"Screen.h"
#include"Input.h"

namespace AEngine
{
	Driver * Driver::GetInstance()
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
		m_initialized = false;
		BaseInput::GetInstance()->Release();
	}

	void Driver::BeginBehaviour(Game::BaseBehaviour * behaviour)
	{
		behaviour->OnInit();
	}
}
