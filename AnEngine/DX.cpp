#include"DX.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace Microsoft::WRL;

namespace AnEngine
{
	D3D12AppBase::D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height) :
		hwnd(_hwnd), width(_width), height(_height), isUseWarpDevice(false)
	{
		WCHAR _assetsPath[512];
		GetAssetsPath(_assetsPath, _countof(_assetsPath));
		assetsPath = _assetsPath;

		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	}

	D3D12AppBase::D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height, const wstring& _title) :
		hwnd(_hwnd), width(_width), height(_height), isUseWarpDevice(false), title(_title)
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
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual m_device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}

	void D3D12AppBase::ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
				_wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
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
}