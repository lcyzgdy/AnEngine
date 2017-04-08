#pragma once
#ifndef __INPUT_H__
#define __INPUT_H__

#define INITGUID

#include"onwind.h"
#include<DirectXMath.h>
#include<dinput.h>
#include<xinput.h>
#include<wrl.h>
#pragma comment(lib, "dinput8.lib")
using namespace Microsoft::WRL;
using namespace DirectX;

enum KeyCode :int
{
	A
};

static class BaseInput
{
	static ComPtr<IDirectInput8> directInput;
	static ComPtr<IDirectInputDevice8> keyboard;
	static ComPtr<IDirectInputDevice8> mouse;
	static HWND hwnd;
	static DIMOUSESTATE2 mouseState;
	static unsigned char keyState[256];

public:
	static void Initialize(HWND _hwnd);
	static void Release();
	static void Update();

	static bool IsAnyKeyDown();
	static bool IsAnyKey();
	static bool IsAnyKeyUp();

	static bool GetKeyDown(int _key);
	static bool GetKey(int _key);
	static bool GetKeyUp(int _key);

	static bool GetMouseButtonDown(int _mouseButton);
	static bool GetMouseButton(int _mouseButton);
	static bool GetMouseButtonUp(int _mouseButton);
	static XMINT2 GetMousePosition();

	static void SetAcquire();
	static void SetUnacquire();

	static void ZeroInputState();
};

#endif // !__INPUT_H__