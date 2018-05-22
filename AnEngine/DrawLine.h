#pragma once
#ifndef __DRAWLINE_H__
#define __DRAWLINE_H__

#include"onwind.h"
#include"DX.h"
#include"Input.h"
#include"RenderCore.h"
#include<stack>
using namespace AnEngine;

class DrawLine :public D3D12AppBase, public D3D12Base
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
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
	// 应用程序资源

	std::vector<Vertex> vertex;

public:
	DrawLine(const HWND _hwnd, const UINT _width, const UINT _height);
	~DrawLine();


	// 通过 D3D12AppBase 继承
	virtual void OnInit() override;

	virtual void OnRelease() override;

	virtual void OnUpdate() override;

	virtual void OnRender() override;

	virtual void OnKeyDown(UINT8 key) override;

	virtual void OnKeyUp(UINT8 key) override;


	// 通过 D3D12Base 继承
	virtual void InitializePipeline() override;

	virtual void InitializeAssets() override;

	virtual void WaitForGpu() override;

	virtual void PopulateCommandList() override;

	virtual void MoveToNextFrame() override;

	virtual void WaitForRenderContext() override;

};

class DrawLineWithWu :public D3D12AppBase, public D3D12Base
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
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
	// 应用程序资源
	Vertex start;
	Vertex end;
	std::vector<Vertex> vertex;

public:
	DrawLineWithWu(const HWND _hwnd, const UINT _width, const UINT _height);
	~DrawLineWithWu();


	// 通过 D3D12AppBase 继承
	virtual void OnInit() override;

	virtual void OnRelease() override;

	virtual void OnUpdate() override;

	virtual void OnRender() override;

	virtual void OnKeyDown(UINT8 key) override;

	virtual void OnKeyUp(UINT8 key) override;


	// 通过 D3D12Base 继承
	virtual void InitializePipeline() override;

	virtual void InitializeAssets() override;

	virtual void WaitForGpu() override;

	virtual void PopulateCommandList() override;

	virtual void MoveToNextFrame() override;

	virtual void WaitForRenderContext() override;
};

class DrawTriangle :public D3D12AppBase, public D3D12Base
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
	ComPtr<ID3D12CommandAllocator> commandAllocators[AnEngine::RenderCore::r_DefaultFrameCount_const];
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	ComPtr<ID3D12Resource> renderTargets[AnEngine::RenderCore::r_DefaultFrameCount_const];
	ComPtr<ID3D12RootSignature> rootSignature;
	UINT rtvDescriptorSize;
	UINT srvDescriptorSize;

	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12PipelineState> pipelineState;

	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT fenceValues[AnEngine::RenderCore::r_DefaultFrameCount_const];

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	// 应用程序资源

	vector<Vertex> vertex;
	stack<int> vertexIndexStack;

	float Cross(Vertex& a, Vertex& b);

	float Cross(Vertex& a, Vertex& b, Vertex& c);

	void Graham();


public:
	DrawTriangle(const HWND _hwnd, const UINT _width, const UINT _height);
	~DrawTriangle();

	// 通过 D3D12AppBase 继承
	virtual void OnInit() override;

	virtual void OnRelease() override;

	virtual void OnUpdate() override;

	virtual void OnRender() override;

	virtual void OnKeyDown(UINT8 key) override;

	virtual void OnKeyUp(UINT8 key) override;


	// 通过 D3D12Base 继承
	virtual void InitializePipeline() override;

	virtual void InitializeAssets() override;

	virtual void WaitForGpu() override;

	virtual void PopulateCommandList() override;

	virtual void MoveToNextFrame() override;

	virtual void WaitForRenderContext() override;

};

#endif // !__DRAWLINE_H__
