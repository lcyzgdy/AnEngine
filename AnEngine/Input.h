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
#include<mutex>

#pragma comment(lib, "dinput8.lib")

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

namespace AnEngine
{
	class BaseInput
	{
		// 2017.10.20
		// Friend functions only can be class's friend in same namespace. So I will redesign in the future
		// 类的友元函数只能位于同一命名空间下。
		// ************************************************************************************************
		friend LRESULT WINAPI::WinProc(HWND, unsigned int, WPARAM, LPARAM);
		friend class Driver;
		//friend int WINAPI ::WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
		//*/

		//friend void RenderCore::InitializeRender(int graphicCardCount, bool isStable);

		Microsoft::WRL::ComPtr<IDirectInput8> m_directInput;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> m_keyboard;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> m_mouse;
		HWND m_hwnd;
		DIMOUSESTATE2 m_mouseState;
		std::atomic<unsigned char> m_keyState[256];
		std::atomic<bool> m_mouseButtonState[10];
		std::atomic<bool> m_mouseButtonDownState[10];
		std::atomic<bool> m_mouseButtonDownFlag[10];
		//atomic<XMVECTOR> m_curPosition;
		//atomic<XMFLOAT3> m_curPosition;
		DirectX::XMFLOAT3 m_curPosition;
		//atomic<BaseInput*> m_baseInput;

		std::atomic_bool m_exit;
		std::chrono::milliseconds m_delta;
		std::mutex m_mutex;
	public:
		BaseInput();
		~BaseInput();
	private:
		void Initialize(HWND _hwnd, HINSTANCE _hInstance);
		void InitializeKeyboard(HINSTANCE _hInstance);
		void InitializeMouse(HINSTANCE _hInstance);
		void Update();
		void Release();

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
		DirectX::XMINT2 GetMousePosition();
		DirectX::XMVECTOR GetM128MousePosition();
		void SetMousePosition(int x, int y);

		void ZeroInputState();

		static BaseInput* GetInstance();
	};

	enum KeyCode :int
	{

	};
}

#endif // !__INPUT_H__