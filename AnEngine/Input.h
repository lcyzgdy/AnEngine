#pragma once
#ifndef __INPUT_H__
#define __INPUT_H__

#define INITGUID

#include "onwind.h"
#include <DirectXMath.h>
#include <dinput.h>
#include <xinput.h>
#include <wrl.h>
#include <atomic>
#include <mutex>
#include "BaseBehaviour.h"

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
		friend class Engine;
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

	enum KeyCode : uint32_t
	{
		None = 0,
		Backspace = 8,
		Tab = 9,
		Clear = 12,
		Return = 13,
		Pause = 19,
		Escape = 27,
		Space = 32,
		Exclaim = 33,
		DoubleQuote = 34,
		Hash = 35,
		Dollar = 36,
		Ampersand = 38,
		Quote = 39,
		LeftParen = 40,
		RightParen = 41,
		Asterisk = 42,
		Plus = 43,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,
		Alpha0 = 48, Alpha1 = 49, Alpha2 = 50, Alpha3 = 51, Alpha4 = 52,
		Alpha5 = 53, Alpha6 = 54, Alpha7 = 55, Alpha8 = 56, Alpha9 = 57,
		Colon = 58, Semicolon = 59,
		Less = 60,
		Equals = 61,
		Greater = 62,
		Question = 63,
		At = 64,
		LeftBracket = 91,
		Backslash = 92,
		RightBracket = 93,
		Caret = 94,
		Underscore = 95,
		BackQuote = 96,
		A = 97, B = 98, C = 99, D = 100, E = 101, F = 102, G = 103, H = 104, I = 105, J = 106, K = 107, L = 108, M = 109, N = 110,
		O = 111, P = 112, Q = 113, R = 114, S = 115, T = 116, U = 117, V = 118, W = 119, X = 120, Y = 121, Z = 122,
		Delete = 127,
		Keypad0 = 256, Keypad1 = 257, Keypad2 = 258, Keypad3 = 259, Keypad4 = 260,
		Keypad5 = 261, Keypad6 = 262, Keypad7 = 263, Keypad8 = 264, Keypad9 = 265,
		KeypadPeriod = 266,
		KeypadDivide = 267,
		KeypadMultiply = 268,
		KeypadMinus = 269,
		KeypadPlus = 270,
		KeypadEnter = 271,
		KeypadEquals = 272,
		UpArrow = 273, DownArrow = 274, RightArrow = 275, LeftArrow = 276,
		Insert = 277,
		Home = 278,
		End = 279,
		PageUp = 280,
		PageDown = 281,
		F1 = 282, F2 = 283, F3 = 284, F4 = 285, F5 = 286, F6 = 287,
		F7 = 288, F8 = 289, F9 = 290, F10 = 291, F11 = 292, F12 = 293,
		F13 = 294, F14 = 295, F15 = 296,
		Numlock = 300,
		CapsLock = 301,
		ScrollLock = 302,
		RightShift = 303,
		LeftShift = 304,
		RightControl = 305,
		LeftControl = 306,
		RightAlt = 307,
		LeftAlt = 308,
		RightApple = 309,
		RightCommand = 309,
		LeftApple = 310,
		LeftCommand = 310,
		LeftWindows = 311,
		RightWindows = 312,
		AltGr = 313,
		Help = 315,
		Print = 316,
		SysReq = 317,
		Break = 318,
		Menu = 319,
	};

	class Input
	{
	public:
		inline static bool GetKey(KeyCode key);
		inline static bool GetKeyDown(KeyCode key);
		inline static bool GetKeyUp(KeyCode key);
	};
}

#endif // !__INPUT_H__