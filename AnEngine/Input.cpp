#include "Input.h"
#include "Screen.h"
#include "ThreadPool.hpp"
using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
#define KeyPress(a) ((m_keyState[a]) & 0x80)

namespace AnEngine
{
	BaseInput::BaseInput() :
		m_directInput(nullptr), m_keyboard(nullptr), m_mouse(nullptr),
		m_hwnd(NULL), m_mouseState{}, m_keyState{}, m_mouseButtonState{ 0,0,0,0,0,0,0,0,0,0 },
		m_mouseButtonDownState{}, m_mouseButtonDownFlag{}, m_curPosition{}, m_exit(false), m_delta(20)
	{
	}

	BaseInput::~BaseInput()
	{
	}

	void BaseInput::Initialize(HWND _hwnd, HINSTANCE _hInstance)
	{
		m_hwnd = _hwnd;

		InitializeKeyboard(_hInstance);
		InitializeMouse(_hInstance);

		memset(m_mouseButtonDownFlag, 0, sizeof(m_mouseButtonDownFlag));
		memset(m_mouseButtonDownState, 0, sizeof(m_mouseButtonDownState));
		memset(m_mouseButtonState, 0, sizeof(m_mouseButtonState));

		//Utility::ThreadPool::Commit(std::bind(&BaseInput::Update, this));
	}

	void BaseInput::InitializeKeyboard(HINSTANCE _hInstance)
	{
		ThrowIfFailed(DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)m_directInput.GetAddressOf(), nullptr));
		ThrowIfFailed(m_directInput->CreateDevice(GUID_SysKeyboard, m_keyboard.GetAddressOf(), nullptr));
		ThrowIfFailed(m_keyboard->SetDataFormat(&c_dfDIKeyboard));
		ThrowIfFailed(m_keyboard->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 10;
		ThrowIfFailed(m_keyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph));

		m_keyboard->Acquire();
	}

	void BaseInput::InitializeMouse(HINSTANCE _hInstance)
	{
		ThrowIfFailed(m_directInput->CreateDevice(GUID_SysMouse, m_mouse.GetAddressOf(), nullptr));
		ThrowIfFailed(m_mouse->SetDataFormat(&c_dfDIMouse2));
		m_mouse->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		m_mouse->Acquire();
	}

	void BaseInput::Release()
	{
		if (m_keyboard)
		{
			m_keyboard->Unacquire();
			//m_keyboard.Get()->Release();		???
			m_keyboard = nullptr;
		}
		if (m_mouse)
		{
			m_mouse->Unacquire();
			//m_mouse.Get()->Release();		???
			m_mouse = nullptr;
		}
		if (m_directInput)
		{
			//m_directInput.Get()->Release();		???
			m_directInput = nullptr;
		}
	}

	void BaseInput::Update()
	{
		//while (!m_exit)
		//{
		if (m_keyboard)
		{
			m_keyboard->GetDeviceState(sizeof(m_keyState), m_keyState);
		}
		if (m_mouse)
		{
			m_mouse->GetDeviceState(sizeof(m_mouseState), &m_mouseState);
			for (int i = 0; i < 4; i++)
			{
				m_mouseButtonState[i] = m_mouseState.rgbButtons[i] & 0x80;
				if ((!m_mouseButtonDownState[i]) && m_mouseButtonState[i] && (!m_mouseButtonDownFlag[i]))
				{
					m_mouseButtonDownState[i] = true;
					m_mouseButtonDownFlag[i] = true;
				}
				else if (m_mouseButtonDownState[i] && m_mouseButtonState[i])
				{
					m_mouseButtonDownState[i] = false;
				}
				else if (!m_mouseButtonState[i])
				{
					m_mouseButtonDownFlag[i] = false;
				}
			}
		}
		//this_thread::sleep_for(m_delta);
	//}
	//m_exit = false;
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
	}

	bool BaseInput::GetKeyUp(int _key)
	{
		return false;
	}

	void BaseInput::UpdateMouseButton(int _button, int _action)
	{
		m_mouseButtonState[_button] = _action;
		m_mouseButtonDownState[_button] = _action;
	}

	bool BaseInput::GetMouseButtonDown(int _mouseButton)
	{
		if (m_mouse)
		{
			/*bool press = mouseState.rgbButtons[_mouseButton] & 0x80;
			if (!mouseButtonDownState[_mouseButton])
			{
				mouseButtonDownState[_mouseButton] = press;
				return press;
			}
			return false;*/
			return m_mouseButtonDownState[_mouseButton];
		}
		else
		{
			//return m_mouseButtonDownState[_mouseButton];
			return false;
		}
	}

	bool BaseInput::GetMouseButton(int _mouseButton)
	{
		//return mouseState.rgbButtons[_mouseButton] & 0x80;
		return m_mouseButtonState[_mouseButton];
		return false;
	}

	bool BaseInput::GetMouseButtonUp(int _mouseButton)
	{
		return true;
	}

	XMINT2 BaseInput::GetMousePosition()
	{
		XMINT2 pos(m_mouseState.lX, m_mouseState.lY);
		return pos;
	}

	XMVECTOR BaseInput::GetM128MousePosition()
	{
		//return XMLoadFloat3(&m_curPosition.load());
		return XMLoadFloat3(&m_curPosition);
	}

	void BaseInput::SetMousePosition(int x, int y)
	{
		m_curPosition = { static_cast<float>(x) / static_cast<float>(Screen::GetInstance()->Width()) * 2 - 1.0f, -(static_cast<float>(y) / static_cast<float>(Screen::GetInstance()->Height()) * 2 - 1.0f), 0.0f };
	}

	void BaseInput::SetAcquire()
	{
		lock_guard<mutex> lock(m_mutex);
		if (m_keyboard)
		{
			m_keyboard->Acquire();
		}
		if (m_mouse)
		{
			m_mouse->Acquire();
		}
	}

	void BaseInput::SetUnacquire()
	{
		lock_guard<mutex> lock(m_mutex);
		if (m_keyboard)
		{
			m_keyboard->Unacquire();
		}
		if (m_mouse)
		{
			m_mouse->Unacquire();
		}
		ZeroInputState();
	}

	void BaseInput::ZeroInputState()
	{
		memset(m_keyState, 0, sizeof(m_keyState));
		memset(&m_mouseState, 0, sizeof(DIMOUSESTATE2));
	}

	BaseInput * BaseInput::GetInstance()
	{
		static BaseInput baseInput;
		return &baseInput;
	}
}