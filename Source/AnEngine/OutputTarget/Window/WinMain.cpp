﻿#include "onwind.h"
#include "NBody.h"
#include "Win32App.h"
#include "ThreadPool.hpp"
#include "Input.h"
#include "Screen.h"
using namespace AnEngine;

#include "Test.h"

WNDCLASSEX wnd;
HWND window;
int state;
int screenw;
int screenh;
Win32App* win32App;


inline LPCWSTR ToLPCWSTR(std::string& orig)
{
	size_t origsize = orig.length() + 1;
	wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (origsize - 1));
	mbstowcs_s(nullptr, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}


inline LPCWSTR ToLPCWSTR(const char* l)
{
	size_t origsize = strlen(l) + 1;
	wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * origsize);
	mbstowcs_s(nullptr, wcstring, origsize, l, _TRUNCATE);
	return wcstring;
}


LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case(WM_CREATE):
	{
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		break;
	}
	case(WM_DESTROY):
	{
		PostQuitMessage(0);
		break;
	}
	case(WM_SETFOCUS):
	{
		BaseInput::Instance().SetAcquire();
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hwnd, &p);
		BaseInput::Instance().SetMousePosition(p.x, p.y);
		break;
	}
	case(WM_KILLFOCUS):
	{
		BaseInput::Instance().SetUnacquire();
		break;
	}
	case(WM_MOUSEMOVE):
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hwnd, &p);
		BaseInput::Instance().SetMousePosition(p.x, p.y);
		break;
	}
	case(WM_PAINT):
	{
		// BaseInput::GetInstance()->Update();	// 这里已经移动到线程池中执行。
		/*if (pD3dApp)
		{
			pD3dApp->OnUpdate();
			pD3dApp->OnRender();
		}*/
		break;
	}
	case(WM_USER):
	{
		var cc = LOWORD(lParam);
		char* l = new char[1];
		l[0] = { (char)(cc + '0') };
		SetWindowText(hwnd, ToLPCWSTR(l));
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	DeleteFile(L"log.txt");
	std::wstring windowTitle(L"AnEngine");
	std::wstring windowClassName(L"AWindow");
	screenw = 1280;
	screenh = 720;

	//D3D12AppBase* d3dApp = new NBody(window, screenw, screenh);
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	//d3dApp->ParseCommandLineArgs(argv, argc);
	LocalFree(argv);

	wnd = { 0 };
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.lpfnWndProc = WinProc;
	wnd.hInstance = hInstance;
	wnd.hIcon = NULL;
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.lpszClassName = windowClassName.c_str();

	RegisterClassEx(&wnd);

	RECT windowRect = { 0, 0, static_cast<LONG>(screenw), static_cast<LONG>(screenh) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	window = CreateWindow(windowClassName.c_str(), windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_BORDER ^ WS_SIZEBOX,		//WS_POPUPWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	//d3dApp);

	if (window == NULL)
	{
		MessageBox(NULL, L"Error", L"Error1", 0);
		state = 1;
		return 0;
	}
	Randomize();
	win32App = new Win32App(window, hInstance, screenw, screenh);
	// AnEngine::Engine::Instance()->Initialize(window, hInstance, screenw, screenh);
	AnEngine::Engine::Instance().Initialize(*win32App);

	//d3dApp->SetHwnd(window);
	//d3dApp->OnInit();
	//Screen::GetInstance()->Initialize(screenw, screenh);
	//BaseInput::GetInstance()->Initialize(window, hInstance);

	//if (!ShowWindow(window, nCmdShow))
	//{
	//	ERRORBLOCK("WINDOWERROR");
	//	return 0;
	//}
	ShowWindow(window, nCmdShow);

	LoadScene();

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Engine::Instance().Release();
	//d3dApp->OnRelease();
	UnregisterClass(wnd.lpszClassName, hInstance);
	return 0;
}