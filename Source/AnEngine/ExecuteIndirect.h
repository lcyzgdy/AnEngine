#pragma once
#ifndef __EXECUTEINDIRECT_H__
#define __EXECUTEINDIRECT_H__

#include"DX.h"
#include"RenderCore.h"
using namespace AnEngine::RenderCore;
using namespace AnEngine;

class ExecuteIndirect :public D3D12AppBase, public D3D12Base
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};
	struct SceneConstantBuffer
	{
		DirectX::XMFLOAT4 velocity;
		DirectX::XMFLOAT4 offset;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4X4 projection;	// 投影
		float padding[36];	// 常量缓冲区是256字节对齐。
	};
	struct CSRootConstants
	{
		float xOffset;
		float zOffset;
		float cullOffset;
		float commandCount;
	};	// 计算着色器的根常量。
	struct IndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbv;
		D3D12_DRAW_ARGUMENTS drawArguments;
	};	// 数据结构匹配ExecuteIndirect的命令签名。
	enum GraphicsRootParameters
	{
		Cbv,
		GraphicsRootParametersCount
	};	// 图形根签名参数偏移量
	enum ComputeRootParameters
	{
		SrvUavTable,
		RootConstants,			// 提供关于三角形顶点和剔除平面的着色器信息的根常量
		ComputeRootParametersCount
	};	// 计算根签名参数偏移量
	enum HeapOffsets
	{
		CbvSrvOffset = 0,													// SRV指向渲染线程使用的常量缓冲区。
		CommandsOffset = CbvSrvOffset + 1,									// SRV指向所有间接命令。
		ProcessedCommandsOffset = CommandsOffset + 1,						// UAV记录实际想要执行的命令
		CbvSrvUavDescriptorCountPerFrame = ProcessedCommandsOffset + 1		// 2个SRV和1个UAV用于计算着色器
	};	// CBV、SRV、UAV描述符堆偏移

	std::vector<SceneConstantBuffer> constantBufferData;
	UINT8* pCbvDataBegin;
	// 每个三角形每帧获得自己的常量缓冲区。

	static const UINT triangleCount = 1024;
	static const UINT triangleResourceCount = triangleCount * r_DefaultFrameCount_const;
	static const UINT commandSizePerFrame;			// 用于在单个框架中绘制所有三角形的间接命令的大小。
	static const UINT commandBufferCounterOffset;	// 在处理命令缓冲区uva计数器的偏移量。
	static const UINT computeThreadBlockSize = 128;	// 应与compute.hlsl中的值匹配。
	static const float triangleHalfWidth;			// 三角形顶点使用的x和y偏移。
	static const float triangleDepth;				// 三角形顶点使用的z偏移。
	static const float cullingCutoff;				// 均匀空间中的剪切平面的±x偏移[-1,1]。

	UINT frameIndex;

	D3D12_RECT cullingScissorRect;
	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[r_DefaultFrameCount_const];
	UINT rtvDescriptorSize;
	UINT dsvDescriptorSize;
	UINT cbvSrvUavDescriptorSize;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSignature;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> computeCommandList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> computeCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> computeCommandAllocators[r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12RootSignature> computeRootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computeState;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	Microsoft::WRL::ComPtr<ID3D12Fence> computeFence;
	UINT64 fenceValues[r_DefaultFrameCount_const];
	HANDLE fenceEvent;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;
	Microsoft::WRL::ComPtr<ID3D12Resource> commandBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> processedCommandBuffers[r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12Resource> processedCommandBufferCounterReset;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	CSRootConstants csRootConstants;	// 计算着色器的常量
	bool enableCulling;	// 计算着色器是否预处理间接命令

	virtual void InitializePipeline();
	virtual void InitializeAssets();
	virtual void PopulateCommandList();
	virtual void WaitForGpu();
	virtual void MoveToNextFrame();
	virtual void WaitForRenderContext();

	static UINT AlignForUavCounter(UINT bufferSize);
	// 我们将UAV计数器打包到与命令相同的缓冲区中，而不是为其创建单独的64K资源堆。
	// 计数器必须在4K边界上对齐，因此我们填充命令缓冲区（如果需要），使得计数器将放置在缓冲区中的有效位置。

public:
	ExecuteIndirect(const HWND _hwnd, const UINT _width, const UINT _height, const wstring& _title);
	~ExecuteIndirect();

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

	virtual void OnKeyUp(UINT8 key);
	virtual void OnKeyDown(UINT8 key);
};

#endif // !__EXECUTEINDIRECT_H__