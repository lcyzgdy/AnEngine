#pragma once
#ifndef __FRAMEBUFFERING_H__
#define __FRAMEBUFFERING_H__

#include"onwind.h"
#include"DX.h"
#include"Input.h"
#include"RenderCore.h"

class FrameBuffering :public AnEngine::D3D12AppBase, public AnEngine::D3D12Base
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	UINT frameIndex;

	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[AnEngine::RenderCore::r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[AnEngine::RenderCore::r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	UINT rtvDescriptorSize;
	UINT srvDescriptorSize;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT fenceValues[AnEngine::RenderCore::r_DefaultFrameCount_const];

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	Microsoft::WRL::ComPtr<ID3D12Resource> cubeResource;
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
