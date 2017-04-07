#pragma once
#ifndef __INPUT_H__
#define __INPUT_H__

#include"onwind.h"
#include<dinput.h>
#include<wrl.h>
#pragma comment(lib,"dinput8.lib")
using namespace Microsoft::WRL;

class BaseInput
{
	ComPtr<IDirectInput8> directInput;
	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInputDevice8> mouse;
	HWND hwnd;
	DIMOUSESTATE2 mouseState;
	unsigned char keyState[256];

public:
	void Initialize(HWND _hwnd);
	void Release();
	void Update();

	bool IsAnyKeyDown();
	bool IsAnyKey();
	bool IsAnyKeyUp();

	bool GetKeyDown(int _key);
	bool GetKey(int _key);
	bool GetKeyUp(int _key);

	bool GetMouseButtonDown(int _mouseButton);
	bool GetMouseButton(int _mouseButton);
	bool GetMouseButtonUp(int _mouseButton);

	void SetAcquire();
	void SetUnacquire();
};

#endif // !__INPUT_H__