#pragma once
#ifndef __RENDERCORE_H__
#define __RENDERCORE_H__

#include"onwind.h"
#include"d3dx12.h"
#include<dxgi1_4.h>
#include<dxgi1_5.h>
#include<dxgi1_6.h>
#include<mutex>
#include<atomic>
using namespace Microsoft::WRL;

namespace RenderCore
{
	static const UINT DefaultFrameCount = 2;
	static const UINT SwapChainBufferCount = 3;
	static constexpr UINT DefaultThreadCount = 1;
	static const DXGI_FORMAT DefaultSwapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
	static const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const D3D_FEATURE_LEVEL MinD3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	static const D3D_FEATURE_LEVEL D3DFeatureLevelWithCreatorUpdate = D3D_FEATURE_LEVEL_12_1;
	static const bool IsUseWarpDevice = false;

	extern bool r_enableHDROutput;

	namespace Private
	{
		ComPtr<IDXGIFactory4> dxgiFactory;
	}

	class CommandQueue
	{
		ComPtr<ID3D12CommandQueue> commandQueue;

		ComPtr<ID3D12Fence> fence;
		atomic_uint64_t nextFenceValue;
		atomic_uint64_t lastCompleteFenceValue;
		HANDLE fenceEvent;

	public:
		CommandQueue() = default;
		~CommandQueue() = default;

		void Initialize(ID3D12Device2* device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		void Release();

		const ID3D12CommandQueue* GetCommandQueue() const;
		D3D12_COMMAND_LIST_TYPE GetType();
	};

	// 显卡设备接口。
	class GraphicCard
	{
		ComPtr<ID3D12Device2> device;

		CommandQueue renderCommandQueue;	// 渲染着色器的命令队列。
		CommandQueue computeCommandQueue;	// 计算着色器的命令队列。
		CommandQueue copyCommandQueue;

		D3D12_FEATURE_DATA_D3D12_OPTIONS featureDataOptions;

		bool isTypedUAVLoadSupport_R11G11B10_FLOAT;
		bool isTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		bool stableFlag;	// 决定GPU是否为稳定的，若为稳定的，则限制供电以避免超频或降频。

		void CreateDevice();
		void CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		inline void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter)
		{
			ComPtr<IDXGIAdapter1> adapter;
			*ppAdapter = nullptr;

			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// 不选择基本显示适配器，如果你想使用软件渲染，在命令行中加入"/warp"
					continue;
				}

				// 检查显卡是否支持DX 12，但是不创建真实的显卡
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
			*ppAdapter = adapter.Detach();
		}

	public:
		GraphicCard();
		GraphicCard(const GraphicCard& graphicCard);
		~GraphicCard() = default;

		void Initialize(bool compute = false, bool copy = false);
		const ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		const ID3D12Device2* GetDevice() const;

		void IsStable(bool isStable);
	};

	extern vector<GraphicCard> r_renderCore;
	extern ComPtr<IDXGISwapChain1> r_swapChain;

	void InitializeRender(int graphicCardCount = 1, bool isStable = false);

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat = DefaultSwapChainFormat);
}


#endif // !__RENDERCORE_H__
