 #pragma once
#ifndef __DX_HPP__
#define __DX_HPP__

#include<windows.h>
#include<iomanip>
#include<iostream>
#include<sstream>
#include<vector>
#include<ctime>
#include<io.h>
#include<wrl.h>
#include"onwind.h"

#include<d3d12.h>
#include"d3dx12.h"
#include<D3Dcompiler.h>
#include<DirectXMath.h>
#include<dxgi1_4.h>
#include<pix.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

namespace DirectY
{
#ifndef __DIRECTY__
#define __DIRECTY__
	static const UINT DefaultFrameCount = 2;
	static const UINT SwapChainBufferCount = 3;
	static const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const D3D_FEATURE_LEVEL D3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	static constexpr UINT DefaultThreadCount = 1;
	
	static ComPtr<ID3D12Device> thisDevice;
	static ComPtr<ID3D12CommandQueue> thisCommandQueue;
	static ComPtr<ID3D12GraphicsCommandList> thisCommandList;
	static ComPtr<ID3D12CommandAllocator> thisCommandAllocator[DefaultFrameCount];

	static ComPtr<ID3D12PipelineState> thisPipelineState;
#endif // !__DIRECTY__
};

using namespace DirectY;

class D3D12AppBase
{
	wstring assetsPath;
	wstring title;
protected:
	bool isUseWarpDevice;
	float aspectRatio;
	HWND hwnd;
	int width;
	int height;

	wstring GetAssetFullPath(LPCWSTR assetName)
	{
		return assetsPath + wstring(assetName);
	}

	void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter)
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
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}

public:
	//D3D12AppBase();
	D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height):
		hwnd(_hwnd), width(_width), height(_height), isUseWarpDevice(false)
	{
		WCHAR _assetsPath[512];
		GetAssetsPath(_assetsPath, _countof(_assetsPath));
		assetsPath = _assetsPath;

		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	}

	D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height, const wstring& _title) :
		hwnd(_hwnd), width(_width), height(_height), isUseWarpDevice(false), title(_title)
	{
		WCHAR _assetsPath[512];
		GetAssetsPath(_assetsPath, _countof(_assetsPath));
		assetsPath = _assetsPath;

		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	}

	virtual ~D3D12AppBase() = default;

	virtual void OnInit() = 0;
	virtual void OnRelease() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

	virtual void OnKeyDown(UINT8 key) = 0;
	virtual void OnKeyUp(UINT8 key) = 0;
	// 事件和委托

	void SetHwnd(const HWND _hwnd)
	{
		hwnd = _hwnd;
	};

	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
				_wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
			{
				isUseWarpDevice = true;
			}
		}
	};

	void SetWindowTitleText(LPCWSTR text) 
	{
		wstring windowText = title + L": " + text;
		SetWindowText(hwnd, windowText.c_str());
	};
};

class D3D12Base
{
protected:
	virtual void InitializePipeline() = 0;
	virtual void InitializeAssets() = 0;
	virtual void WaitForGpu() = 0;
	virtual void PopulateCommandList() = 0;
	virtual void MoveToNextFrame() = 0;
	virtual void WaitForRenderContext() = 0;
public:
	D3D12Base() = default;
	~D3D12Base() = default;
};

#endif // !__DX_HPP__
