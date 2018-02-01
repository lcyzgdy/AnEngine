#pragma once
#ifndef __FRAMEBUFFERING_H__
#define __FRAMEBUFFERING_H__

#include"onwind.h"
#include"DX.h"
#include"Input.h"
#include"RenderCore.h"
using namespace AnEngine::RenderCore;
using namespace AnEngine;

class FrameBuffering :public D3D12AppBase, public D3D12Base
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	UINT frameIndex;

	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocators[r_DefaultFrameCount_const];
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	ComPtr<ID3D12Resource> renderTargets[r_DefaultFrameCount_const];
	ComPtr<ID3D12RootSignature> rootSignature;
	UINT rtvDescriptorSize;
	UINT srvDescriptorSize;

	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12PipelineState> pipelineState;

	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT fenceValues[r_DefaultFrameCount_const];

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	ComPtr<ID3D12Resource> cubeResource;
	// 应用程序资源


public:
	FrameBuffering(const HWND _hwnd, const UINT _width, const UINT _height);
	~FrameBuffering();

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

	virtual void OnKeyUp(UINT8 _key);
	virtual void OnKeyDown(UINT8 _key);

	virtual void InitializePipeline();
	virtual void InitializeAssets();
	virtual void PopulateCommandList();
	virtual void WaitForGpu();
	virtual void MoveToNextFrame();
	virtual void WaitForRenderContext();
};

#endif // !__FRAMEBUFFERING_H__
