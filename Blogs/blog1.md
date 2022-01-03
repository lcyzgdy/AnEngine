# 一个引擎系列

这是一个使用C++和DirectX 12编写的简单的游戏引擎。关于DirectX 12的文章已经有很多，此处不再进行介绍。

### 交换链、设备、窗口
抛开所有，引擎工作需要在显卡上进行渲染，并将渲染的内容通过交换链呈现在一个窗口上。窗口是Windows系统上一个相当普遍的概念，而交换链是指工作在DX和Windows之间的一个抽象层，负责两者间的交互。DX12由于支持了Multi Adapter特性，设备是基于一个适配器创建的。一个适配器下可以是一个物理GPU，也可以是多个物理GPU，也可以是抽象的Warp显示设备，因此适配器是对物理设备的一个抽象。私以为交换链是DX和Windows的中间件，因此可以这样想：交换链的后端是DX，而交换链的前端可以是Win32窗口或UWP窗口，对应交换链创建的两个方法：CreateSwapChainForHwnd和CreateSwapChainForCoreWindow。

### 创建一个Win32窗口
该引擎还没有添加UWP支持，所以只创建一个Win32窗口。窗口创建方式和Win32应用一样。
``` cpp
#include"onwind.h"

WNDCLASSEX wnd;
HWND window;
int state;
int screenw;
int screenh;

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
	case(WM_PAINT):
	{
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::wstring windowTitle(_T("AnEngine"));
	std::wstring windowClassName(_T("Window"));
	screenw = 1280;
	screenh = 720;

	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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

	window = CreateWindow(
		windowClassName.c_str(),
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_BORDER ^ WS_SIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (window == NULL)
	{
		MessageBox(NULL, _T("Error"), _T("Error1"), 0);
		state = 1;
		return 0;
	}

	ShowWindow(window, nCmdShow);

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass(wnd.lpszClassName, hInstance);
	return 0;
}
```
这样就创建了一个1280*720的窗口，窗口矩形区域通过AdjustWindowRect指定，所以这个分辨率指的是客户区，既窗口中不含标题栏的部分。由于游戏和普通的桌面应用不同，所以在消息循环时使用了比GetMessage更好的PeekMessage，可以主动从消息队列中获取消息，性能优于GetMessage。

##### onwind.h
这是我写的一个辅助性的头文件，其中包含了大部分头文件的引用，包括Windows.h，也做了一些针对不同平台的函数重载。比如：
``` cpp
inline LPCWSTR ToLPCWSTR(std::string& orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t) *(origsize - 1));
	mbstowcs_s(nullptr, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}
```
在Win32和Com中有许多需要字符串参数的地方，在Unicode字符集下需要类型为LPCWSTR(wchar_t\*)，而多字节字符集需要LPCSTR(char\*)，因此需要将string进行格式转换。Win32和Com的一些接口的返回值为HRESULT，如果返回S_OK则说明接口调用成功，所以在失败的时候需要抛出一个异常（因为有时需要在try-catch中针对异常进行处理）。

### 小结
以上，一个引擎的窗口就做好了，但是没有任何显示的功能。因此接下来就需要做些什么来让其显示。接下来WinMain和WinProc需要改动的地方不多，毕竟DirectX 12更加底层。