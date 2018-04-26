#pragma once
#ifndef __GRAPHICCARD_H__
#define __GRAPHICCARD_H__

#include"DX.h"
#include"CommandQueue.h"
#include"RenderCoreConstants.h"
#include<mutex>
//#include"Fence.h"

namespace AnEngine::RenderCore
{
	// 显卡设备接口。
	class GraphicsCard : public NonCopyable
	{
		friend class UICore;

		Microsoft::WRL::ComPtr<ID3D12Device2> m_device_cp;

		// 渲染着色器的命令队列。
		CommandQueue m_renderCommandQueue;
		// 计算着色器的命令队列。
		CommandQueue m_computeCommandQueue;
		// 拷贝命令队列
		CommandQueue m_copyCommandQueue;

		//Fence m_fence;
		//Microsoft::WRL::ComPtr<ID3D12Fence> m_fence_cp;

		D3D12_FEATURE_DATA_D3D12_OPTIONS m_featureDataOptions;

		bool m_isTypedUAVLoadSupport_R11G11B10_FLOAT;
		bool m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		// 决定GPU是否为稳定的，若为稳定的，则限制供电以避免超频或降频。
		bool m_stableFlag;
		uint32_t m_node;

		std::mutex m_execMutex;

		void CreateDevice(IDXGIFactory4* dxgiFactory);
		void CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		inline void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
			*ppAdapter = nullptr;

			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// 不选择基本显示适配器，如果想使用软件渲染，在命令行中加入"/warp"
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
		GraphicsCard();
		~GraphicsCard() = default;

		void Initialize(IDXGIFactory4* dxgiFactory, bool compute = false, bool copy = false);
		const ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		const ID3D12Device* GetDevice() const;
		ID3D12Device* GetDevice();
		uint32_t GetNodeNum();

		void ExecuteSync(_In_ uint32_t num, _In_reads_(num) ID3D12CommandList *const *ppCommandLists, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		void IsStable(bool isStable);

		void GpuWait(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		void GpuSignal(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		HRESULT GetDeviceRemovedReason();
	};
}

#endif // !__GRAPHICCARD_H__
