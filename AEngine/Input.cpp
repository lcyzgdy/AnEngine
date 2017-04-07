#include "Input.h"

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
	mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
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
		mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);
	}
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
	return false;
}

bool BaseInput::GetKeyUp(int _key)
{
	return false;
}

bool BaseInput::GetMouseButtonDown(int _mouseButton)
{
	return false;
}

bool BaseInput::GetMouseButton(int _mouseButton)
{
	return false;
}

bool BaseInput::GetMouseButtonUp(int _mouseButton)
{
	return false;
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
}
