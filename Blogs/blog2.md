# 一个引擎
上一篇已经成功创建一个窗口，于是我们的工作重心该转回D3D了。
### 运行状态
假如我们把D3D有关的内容放到一个类D3DApp中，那么为了完成渲染任务，它需要有以下几个阶段：
```cpp
class D3DApp
{
public:
    void OnInit()
    {
        // 对象初始化时的代码。
    }

    void OnUpdate()
    {
        // 每一帧更新时在CPU上执行的代码，一般是游戏逻辑。
    }

    void OnRender()
    {
        // 每一帧更新时在GPU上执行的代码，与D3D相关的代码大部分放在这里。
    }

    void OnRelease()
    {
        // 对象在释放时执行的代码
    }
}
```
``` cpp
D3DApp* pD3DApp = new D3DApp();
pD3DApp->OnInit();
while (!exit)
{
    pD3DApp->OnUpdate();
    pD3DApp->OnRender();
}
pD3DApp->OnRelease();
```
大致就是这么个逻辑，因此我定义了一个抽象基类D3D12AppBase来描述这些行为。
### D3D12AppBase
新建头文件DX.h，首先要解决一些头文件引用和静态库的问题。
``` cpp
#include<d3d12.h>
#include"d3dx12.h"
#include<D3Dcompiler.h>
#include<DirectXMath.h>
#include<dxgi1_4.h>
#include<pix.h>
#include<dcomp.h>

#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
```
* d3d12.h：主要的头文件，包括与DX有关的最基本的接口、类型等。
* d3dx12.h：微软给出的DX辅助用头文件，主要是对DX一些接口、函数和类型的封装。从某一版本开始不再将D3DX作为DX的一部分，需要到<a href="https://github.com/Microsoft/DirectX-Graphics-Samples/tree/master/Libraries/D3DX12">GitHub上自行下载</a>。
* D3Dcompiler.h：编译Shader的相关类库。
* DirectXMath.h：DX程序必备的数学库，到DX12时已经兼备XNA和原数学库的优点。其中大部分函数使用SSE指令集实现，效率很高。
* dxgi1_4.h：有多个版本，目前最新的为1.6，1.6版本针对Windows 10秋季创意者更新，有许多老版本Win10不兼容的特性，因此暂时为了兼容使用1.4。DXGI(DirectX Graphics Infrastructure)是沟通硬件和操作系统的图形基础架构。
* pix.h、dcomp.h：由于DX支持Com，所以使用ComPtr对其进行管理。这里不再过多介绍Com。

然后需要链接一些静态库，这些静态库也可以在“项目->属性->链接器->附加依赖项”中添加这些静态库。

另外尽量不要在头文件中using namespace，否则在所有包含头文件的CPP文件中都会using namespace，会带来一些意想不到的重定义、命名冲突等诡异的错误。
``` cpp
class D3D12AppBase
{
	std::wstring assetsPath;
	std::wstring title;
protected:
	bool isUseWarpDevice;
	float aspectRatio;
	HWND hwnd;
	int width;
	int height;
	inline std::wstring GetAssetFullPath(LPCWSTR assetName)
	{
		return assetsPath + std::wstring(assetName);
	}
	void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);
public:
	D3D12AppBase(const HWND _hwnd, const uint32_t _width, const uint32_t height);
	D3D12AppBase(const HWND _hwnd, const uint32_t _width, const uint32_t height, const std::wstring& _title);
	virtual ~D3D12AppBase() = default;
	virtual void OnInit() = 0;
	virtual void OnRelease() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnKeyDown(uint8_t key) = 0;
	virtual void OnKeyUp(uint8_t key) = 0;
	void SetHwnd(const HWND _hwnd);
	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);
	void SetWindowTitleText(LPCWSTR text);
};
```
``` cpp
D3D12AppBase::D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height) : hwnd(_hwnd), width(_width), height(_height), isUseWarpDevice(false)
{
    WCHAR _assetsPath[512];
    GetAssetsPath(_assetsPath, _countof(_assetsPath));
    assetsPath = _assetsPath;
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

D3D12AppBase::D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height, const wstring& _title) : hwnd(_hwnd), width(_width), height(_height), isUseWarpDevice(false), title(_title)
{
    WCHAR _assetsPath[512];
    GetAssetsPath(_assetsPath, _countof(_assetsPath));
    assetsPath = _assetsPath;
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void D3D12AppBase::GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter)
{
    ComPtr<IDXGIAdapter1> adapter;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
    {
        // 枚举显示适配器
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // 如果有需要，可以选择Warp适配器，既Microsoft基本显示适配器。
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            // 否则从枚举到的第一个设备上创建。这里的设备顺序和系统设置有关，比如在笔记本电脑上常见配置为Intel核显+Nvidia独显，在NVidia控制面板里可以选择程序默认显卡，如果是“集成式”此处第一个被枚举的设备是核显。
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}

void D3D12AppBase::ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc)
{
    for (int i = 1; i < argc; ++i)
    {
        if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 || _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
        {
            isUseWarpDevice = true;
        }
    }
}

void D3D12AppBase::SetHwnd(const HWND _hwnd)
{
    hwnd = _hwnd;
}

void D3D12AppBase::SetWindowTitleText(LPCWSTR text)
{
    wstring windowText = title + L": " + text;
    SetWindowText(hwnd, windowText.c_str());
}
```
如上，一个基本的D3D12应用架构就出来了。