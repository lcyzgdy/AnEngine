#pragma once
#ifndef __CUBE_H__
#define __CUBE_H__

#include"onwind.h"
#include"DX.h"
#include"RenderCore.h"
#include"MCamera.h"
#include"DTimer.h"
#include"DynamicConstantBuffer.h"
using namespace AEngine;

class DrawCube :public D3D12AppBase, public D3D12Base
{
	struct VertexPositionColor
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct VertexPositionUV
	{
		XMFLOAT4 position;
		XMFLOAT2 uv;
	};

	struct DrawConstantBuffer
	{
		XMMATRIX worldViewProjection;
	};

	enum RootParameters : UINT32
	{
		RootParameterUberShaderCB = 0,
		RootParameterCB,
		RootParameterSRV,
		RootParametersCount
	};

	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocators[r_cnt_DefaultFrameCount];
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12DescriptorHeap> samHeap;
	ComPtr<ID3D12Resource> renderTargets[r_cnt_DefaultFrameCount];
	ComPtr<ID3D12RootSignature> rootSignature;
	UINT rtvDescriptorSize;
	UINT srvDescriptorSize;
	UINT dsvDescriptorSize;
	UINT samDescriptorSize;
	UINT cbvSrvUavDescriptorSize;

	ComPtr<IDCompositionDevice> dCompositionDevice;
	ComPtr<IDCompositionTarget> dCompositionTarget;
	ComPtr<IDCompositionVisual> dCompositionVisual;

	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12PipelineState> pipelineState;

	UINT frameIndex;

	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	HANDLE swapChainEvent;
	UINT fenceValues[r_cnt_DefaultFrameCount];

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleCSU[1];
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleDSV[1];

	ComPtr<ID3D12Resource> cube;
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> vertexIndexBuffer;
	ComPtr<ID3D12Resource> cBufferMatrix;
	ComPtr<ID3D12Resource> depthBuffer;
	D3D12_VERTEX_BUFFER_VIEW cubeVbv;
	D3D12_INDEX_BUFFER_VIEW cubeIbv;
	XMFLOAT4X4  cBMatrix;
	// 应用程序资源

	vector<VertexPositionColor> vertex;

	MCamera camera;
	XMMATRIX projectionMatrix;
	DTimer timer;

public:
	DrawCube(const HWND _hwnd, const UINT _width, const UINT _height);
	~DrawCube() = default;


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

#endif // !__CUBE_H__
