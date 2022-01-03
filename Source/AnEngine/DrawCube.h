#pragma once
#ifndef __CUBE_H__
#define __CUBE_H__

#include"onwind.h"
#include"DX.h"
#include"RenderCore.h"
#include"Camera.h"
#include"DTimer.h"
#include"DynamicConstantBuffer.h"
using namespace AnEngine;

class DrawCube :public D3D12AppBase, public D3D12Base
{
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	struct VertexPositionUV
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 uv;
	};

	struct DrawConstantBuffer
	{
		DirectX::XMMATRIX worldViewProjection;
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
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[AnEngine::RenderCore::r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> samHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[AnEngine::RenderCore::r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	UINT rtvDescriptorSize;
	UINT srvDescriptorSize;
	UINT dsvDescriptorSize;
	UINT samDescriptorSize;
	UINT cbvSrvUavDescriptorSize;

	Microsoft::WRL::ComPtr<IDCompositionDevice> dCompositionDevice;
	Microsoft::WRL::ComPtr<IDCompositionTarget> dCompositionTarget;
	Microsoft::WRL::ComPtr<IDCompositionVisual> dCompositionVisual;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

	UINT frameIndex;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	HANDLE swapChainEvent;
	UINT fenceValues[AnEngine::RenderCore::r_DefaultFrameCount_const];

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleCSU[1];
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleDSV[1];

	Microsoft::WRL::ComPtr<ID3D12Resource> cube;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> cBufferMatrix;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	D3D12_VERTEX_BUFFER_VIEW cubeVbv;
	D3D12_INDEX_BUFFER_VIEW cubeIbv;
	DirectX::XMFLOAT4X4  cBMatrix;
	// 应用程序资源

	std::vector<VertexPositionColor> vertex;

	AnEngine::Game::Camera camera;
	DirectX::XMMATRIX projectionMatrix;
	//DTimer timer;

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
