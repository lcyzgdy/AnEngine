#pragma once
#ifndef __NBODY_H__
#define __NBODY_H__

#include"DX.h"
#include"DTimer.h"
#include"RenderCore.h"
#define InterlockedGetValue(obj) InterlockedCompareExchange(obj, 0, 0)
using namespace AnEngine;

class NBody :public D3D12AppBase, public D3D12Base
{
	static const UINT ParticleCount = 12000;
	static const float ParticleSpread;
	static const UINT ThreadCount = RenderCore::r_DefaultThreadCount_const;

	UINT frameIndex;

	struct Particle
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 velocity;
	};

	struct ParticleVertex
	{
		DirectX::XMFLOAT4 color;
	};

	struct ConstantBufferGS
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 inverseView;

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
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvUavHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[RenderCore::r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[RenderCore::r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	UINT rtvDescriptorSize;
	UINT srvUavDescriptorSize;
	// 管线对象

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computeState;
	// 资源对象

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	Microsoft::WRL::ComPtr<ID3D12Fence> computeFence;
	HANDLE fenceEvent;
	HANDLE swapChainEvent;
	UINT64 fenceValues[RenderCore::r_DefaultFrameCount_const];
	UINT64 renderContextFenceValue;
	HANDLE renderContextFenceEvent;
	// 同步对象

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> computeCommandQueue[ThreadCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> computeCommandList[ThreadCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> computeCommandAllocators[RenderCore::r_DefaultFrameCount_const];
	Microsoft::WRL::ComPtr<ID3D12RootSignature>	computeRootSignature;
	// 计算对象

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUpload;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	Microsoft::WRL::ComPtr<ID3D12Resource> particleBuffer0[ThreadCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> particleBuffer1[ThreadCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> particleBuffer0Upload[ThreadCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> particleBuffer1Upload[ThreadCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferGS;
	UINT8* pConstantBufferGSData;
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferCS;

	UINT srvIndex[ThreadCount];		// 表示哪个例子的缓冲资源试图是SRV（0 || 1）（UAV = 1 - SRV）
	UINT heightInstances;
	UINT widthInstances;
	// MiCamera camera;
	//DTimer timer;

	Microsoft::WRL::ComPtr<ID3D12Fence> threadFences[ThreadCount];
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
	void InitializeParticles(_Out_writes_(_particlesNum) Particle* _pParticles, const DirectX::XMFLOAT3& _center, 
		const DirectX::XMFLOAT4& _velocity, float _spread, UINT _particlesNum);
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
