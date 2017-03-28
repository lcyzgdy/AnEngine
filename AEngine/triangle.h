#pragma once
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include"onwind.h"
#include"DX.h"
using namespace std;
using namespace DirectX;

class Triangle :public D3D12AppBase, public D3D12Base
{
public:
	Triangle(const HWND _hwnd, const int _width, const int _height);
	~Triangle();

	virtual void OnInit();
	virtual void OnRelease();
	virtual void OnUpdate();
	virtual void OnRender();

	virtual void OnKeyUp();
	virtual void OnKeyDown();

private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};// A struct

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	// 应用程序资源

	virtual void InitializePipeline();
	virtual void InitializeAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
	void WaitForGpu();
	void NextFrame();
};

#endif // !__TRIANGLE_H__
