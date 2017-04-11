#pragma once
#ifndef __NBODY_H__
#define __NBODY_H__

#include"DX.h"
#include"DTimer.h"
#include"MCamera.h"
#define InterlockedGetValue(obj) InterlockedCompareExchange(obj, 0, 0)

class NBody :public D3D12AppBase, public D3D12Base
{
	static const UINT ParticleCount = 8000;
	static const float ParticleSpread;
	static const UINT ThreadCount = DefaultThreadCount;

	UINT frameIndex;

	struct Particle
	{
		XMFLOAT4 position;
		XMFLOAT4 velocity;
	};

	struct ParticleVertex
	{
		XMFLOAT4 color;
	};

	struct ConstantBufferGS
	{
		XMFLOAT4X4 worldViewProjection;
		XMFLOAT4X4 inverseView;

		float padding[32];
		// 常量缓冲区在显存中以256字节对齐。
	};

	struct ConstantBufferCS
	{
		UINT param[4];
		float paramf[4];
	};

	struct ThreadData
	{
		NBody* pContext;
		UINT threadIndex;
	};	// 此处重构：线程管理模板

	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;
	D3D12_RECT cullingScissorRect;	// 用作遮挡剔除
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> srvUavHeap;
	ComPtr<ID3D12Resource> renderTargets[DefaultFrameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocators[DefaultFrameCount];
	ComPtr<ID3D12RootSignature> rootSignature;
	UINT rtvDescriptorSize;
	UINT srvUavDescriptorSize;
	// 管线对象

	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12PipelineState> computeState;
	// 资源对象

	ComPtr<ID3D12Fence> fence;
	ComPtr<ID3D12Fence> computeFence;
	HANDLE fenceEvent;
	HANDLE swapChainEvent;
	UINT64 fenceValues[DefaultFrameCount];
	UINT64 renderContextFenceValue;
	HANDLE renderContextFenceEvent;
	// 同步对象

	ComPtr<ID3D12CommandQueue> computeCommandQueue[ThreadCount];
	ComPtr<ID3D12GraphicsCommandList> computeCommandList[ThreadCount];
	ComPtr<ID3D12CommandAllocator> computeCommandAllocators[DefaultFrameCount];
	ComPtr<ID3D12RootSignature>	computeRootSignature;
	// 计算对象

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> vertexBufferUpload;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPtr<ID3D12Resource> particleBuffer0[ThreadCount];
	ComPtr<ID3D12Resource> particleBuffer1[ThreadCount];
	ComPtr<ID3D12Resource> particleBuffer0Upload[ThreadCount];
	ComPtr<ID3D12Resource> particleBuffer1Upload[ThreadCount];
	ComPtr<ID3D12Resource> constantBufferGS;
	UINT8* pConstantBufferGSData;
	ComPtr<ID3D12Resource> constantBufferCS;

	UINT srvIndex[ThreadCount];		// 表示哪个例子的缓冲资源试图是SRV（0 || 1）（UAV = 1 - SRV）
	UINT heightInstances;
	UINT widthInstances;
	MCamera camera;
	DTimer timer;

	ComPtr<ID3D12Fence> threadFences[ThreadCount];
	volatile HANDLE threadFenceEvents[ThreadCount];
	// 线程同步对象

	LONG volatile terminating;
	UINT64 volatile renderContextFenceValues[ThreadCount];
	UINT64 volatile threadFenceValues[ThreadCount];
	// 线程状态

	ThreadData threadData[ThreadCount];
	HANDLE threadHandles[ThreadCount];

	enum GraphicsRootParameters :UINT32
	{
		GraphicsRootCbv = 0,
		GraphicsRootSrvTable,
		GraphicsRootParametersCount
	};

	enum ComputeRootParameters :UINT32
	{
		ComputeRootCbv = 0,
		ComputeRootSrvTable,
		ComputeRootUavTable,
		ComputeRootParametersCount
	};

	enum DescriptorHeapIndex : UINT32
	{
		UavParticlePosVelo0 = 0,
		UavParticlePosVelo1 = UavParticlePosVelo0 + ThreadCount,
		SrvParticlePosVelo0 = UavParticlePosVelo1 + ThreadCount,
		SrvParticlePosVelo1 = SrvParticlePosVelo0 + ThreadCount,
		DescriptorCount = SrvParticlePosVelo1 + ThreadCount
	};	// 着色器资源的索引

	void InitializePipeline();
	void InitializeAssets();
	void PopulateCommandList();
	void WaitForGpu();
	void WaitForRenderContext();
	void MoveToNextFrame();

	void CreateAsyncContexts();
	void CreateVertexBuffer();
	float RandomPercent();	// (-1, 1)
	void InitializeParticles(_Out_writes_(_particlesNum) Particle* _pParticles, const XMFLOAT3 &_center, const XMFLOAT4 &_velocity, float _spread, UINT _particlesNum);
	void CreateParticlesBuffer();

	static DWORD WINAPI ThreadProc(ThreadData* _pThread);
	DWORD AsyncComputeThreadProc(int _threadIndex);
	void Simulate(UINT _threadIndex);

public:
	NBody(const HWND _hwnd, const UINT _width, const UINT _height);
	~NBody();

	void OnInit();
	void OnUpdate();
	void OnRender();
	void OnRelease();

	void OnKeyUp(UINT8 _key);
	void OnKeyDown(UINT8 _key);
};

#endif // !__NBODY_H__
