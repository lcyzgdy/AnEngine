#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__


#include"DX.h"
#include"onwind.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

class Texture :public D3D12AppBase, public D3D12Base
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};// A struct

	static const UINT textureWidth = 256;
	static const UINT textureHeight = 256;
	static const UINT texturePixelSize = 4;

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPtr<ID3D12Resource> texture;

	vector<UINT8> GenerateTextureData();

public:
	Texture(const HWND _hwnd, const int _width, const int _height);
	~Texture();

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

	virtual void OnKeyUp();
	virtual void OnKeyDown();

	virtual void InitializePipeline();
	virtual void InitializeAssets();
	virtual void PopulateCommandList();
};

#endif // !__TEXTURE_H__