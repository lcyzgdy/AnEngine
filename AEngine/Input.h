#pragma once
#ifndef __INPUT_H__
#define __INPUT_H__

#define INITGUID

#include"onwind.h"
#include<DirectXMath.h>
#include<dinput.h>
#include<xinput.h>
#include<wrl.h>
#include<atomic>
#include"ThreadPool.hpp"

#pragma comment(lib, "dinput8.lib")
using namespace Microsoft::WRL;
using namespace DirectX;

class BaseInput : public NonCopyable
{
	friend LRESULT WINAPI WinProc(HWND hwnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
	friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	//friend void RenderCore::InitializeRender(int graphicCardCount, bool isStable);

	ComPtr<IDirectInput8> m_directInput;
	ComPtr<IDirectInputDevice8> m_keyboard;
	ComPtr<IDirectInputDevice8> m_mouse;
	HWND m_hwnd;
	DIMOUSESTATE2 m_mouseState;
	atomic<unsigned char> m_keyState[256];
	atomic<bool> m_mouseButtonState[10];
	atomic<bool> m_mouseButtonDownState[10];
	atomic<bool> m_mouseButtonDownFlag[10];
	atomic<XMVECTOR> m_curPosition;
	atomic<BaseInput*> m_baseInput;

	atomic_bool m_exit;
	std::chrono::milliseconds m_delta;
	mutex m_mutex;

	BaseInput();
	BaseInput(const BaseInput&) = delete;
	~BaseInput();

	void Initialize(HWND _hwnd, HINSTANCE _hInstance);
	void InitializeKeyboard(HINSTANCE _hInstance);
	void InitializeMouse(HINSTANCE _hInstance);
	void Release();
	void Update();

	void SetAcquire();
	void SetUnacquire();

public:
	bool IsAnyKeyDown();
	bool IsAnyKey();
	bool IsAnyKeyUp();

	bool GetKeyDown(int _key);
	bool GetKey(int _key);
	bool GetKeyUp(int _key);

	void UpdateMouseButton(int _button, int _action);
	bool GetMouseButtonDown(int _mouseButton);
	bool GetMouseButton(int _mouseButton);
	bool GetMouseButtonUp(int _mouseButton);
	XMINT2 GetMousePosition();
	XMVECTOR GetM128MousePosition();
	void SetMousePosition(int x, int y);

	void ZeroInputState();

	static BaseInput* GetInstance();
};



#endif // !__INPUT_H__