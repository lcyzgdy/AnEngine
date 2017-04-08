#include "Input.h"
#define KeyPress(a) ((keyState[a]) & 0x80)

ComPtr<IDirectInput8> BaseInput::directInput = nullptr;
ComPtr<IDirectInputDevice8> BaseInput::keyboard = nullptr;
ComPtr<IDirectInputDevice8> BaseInput::mouse = nullptr;
HWND BaseInput::hwnd = NULL;
DIMOUSESTATE2 BaseInput::mouseState = {};
unsigned char BaseInput::keyState[256] = {};
bool BaseInput::mouseButtonState[10] = { 0,0,0,0,0,0,0,0,0,0 };

void BaseInput::Initialize(HWND _hwnd)
{
	hwnd = _hwnd;
	ThrowIfFailed(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)directInput.GetAddressOf(), nullptr));
	ThrowIfFailed(directInput->CreateDevice(GUID_SysKeyboard, keyboard.GetAddressOf(), nullptr));
	ThrowIfFailed(keyboard->SetDataFormat(&c_dfDIKeyboard));
	ThrowIfFailed(keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 10;
	ThrowIfFailed(keyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph));

	ThrowIfFailed(directInput->CreateDevice(GUID_SysMouse, mouse.GetAddressOf(), nullptr));
	ThrowIfFailed(mouse->SetDataFormat(&c_dfDIMouse2));
	mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	keyboard->Acquire();
	mouse->Acquire();
}

void BaseInput::Release()
{
	if (keyboard)
	{
		keyboard->Unacquire();
		//keyboard.Get()->Release();
		keyboard = nullptr;
	}
	if (mouse)
	{
		mouse->Unacquire();
		//mouse.Get()->Release();
		mouse = nullptr;
	}
	if (directInput)
	{
		//directInput.Get()->Release();
		directInput = nullptr;
	}
}

void BaseInput::Update()
{
	if (keyboard)
	{
		keyboard->GetDeviceState(sizeof(keyState), keyState);
	}
	if (mouse)
	{
		mouse->GetDeviceState(sizeof(mouseState), &mouseState);
	}
	/*
	for (int i = 0; i < 8; ++i)
	{
		mouseButtonState[i] = mouseState.rgbButtons[i] & 0x80;
	}*/
	memset(mouseButtonState, 0, sizeof(mouseButtonState));
}

bool BaseInput::IsAnyKeyDown()
{
	return false;
}

bool BaseInput::IsAnyKey()
{
	return false;
}

bool BaseInput::IsAnyKeyUp()
{
	return false;
}

bool BaseInput::GetKeyDown(int _key)
{
	return false;
}

bool BaseInput::GetKey(int _key)
{
	return KeyPress(_key);
	return false;
}

bool BaseInput::GetKeyUp(int _key)
{
	return false;
}

bool BaseInput::GetMouseButtonDown(int _mouseButton)
{
	bool press = mouseState.rgbButtons[_mouseButton] & 0x80;
	if (!mouseButtonState[_mouseButton])
	{
		mouseButtonState[_mouseButton] = press;
		return press;
	}
	return false;
}

bool BaseInput::GetMouseButton(int _mouseButton)
{
	return mouseState.rgbButtons[_mouseButton] & 0x80;
	return false;
}

bool BaseInput::GetMouseButtonUp(int _mouseButton)
{
	return false;
}

XMINT2 BaseInput::GetMousePosition()
{
	XMINT2 pos(mouseState.lX, mouseState.lY);
	return pos;
}

void BaseInput::SetAcquire()
{
	if (keyboard)
	{
		keyboard->Acquire();
	}
	if (mouse)
	{
		mouse->Acquire();
	}
}

void BaseInput::SetUnacquire()
{
	if (keyboard)
	{
		keyboard->Unacquire();
	}
	if (mouse)
	{
		mouse->Unacquire();
	}
	ZeroInputState();
}

void BaseInput::ZeroInputState()
{
	memset(keyState, 0,sizeof(keyState));
	memset(&mouseState, 0, sizeof(DIMOUSESTATE2));
}
