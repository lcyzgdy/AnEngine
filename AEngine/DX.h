#pragma once
#pragma once
#ifndef __DX_H__
#define __DX_H__

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
#include<dcomp.h>

#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

#ifdef _WIN64

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

	inline wstring GetAssetFullPath(LPCWSTR assetName)
	{
		return assetsPath + wstring(assetName);
	}

	void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);

public:
	D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height);

	D3D12AppBase(const HWND _hwnd, const UINT _width, const UINT _height, const wstring& _title);

	virtual ~D3D12AppBase() = default;

	virtual void OnInit() = 0;
	virtual void OnRelease() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

	virtual void OnKeyDown(UINT8 key) = 0;
	virtual void OnKeyUp(UINT8 key) = 0;
	// 事件和委托

	void SetHwnd(const HWND _hwnd);

	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

	void D3D12AppBase::SetWindowTitleText(LPCWSTR text);
};
#endif // !_WIN64

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
#endif // !__DX_H__
