#pragma once
#ifndef __GRAPHICSCARD_H__
#define __GRAPHICSCARD_H__

#include "DX.h"
#include "CommandQueue.h"
#include "RenderCoreConstants.h"
#include <mutex>
//#include"Fence.h"

namespace AnEngine::RenderCore
{
	// 显卡设备接口。
	class GraphicsCard_Obsolete : public NonCopyable
	{
		std::string m_name;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_device_cp;

		// 渲染着色器的命令队列。
		CommandQueue m_renderCommandQueue;
		// 计算着色器的命令队列。
		CommandQueue m_computeCommandQueue;
		// 拷贝命令队列
		CommandQueue m_copyCommandQueue;

		D3D12_FEATURE_DATA_D3D12_OPTIONS m_featureDataOptions;

		bool m_isTypedUAVLoadSupport_R11G11B10_FLOAT;
		bool m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		// 决定GPU是否为稳定的，若为稳定的，则限制供电以避免超频或降频。
		bool m_stableFlag;
		uint32_t m_node;

		std::mutex m_execMutex;

	protected:
		virtual void CreateDevice(IDXGIFactory4* dxgiFactory);
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
		GraphicsCard_Obsolete();
		virtual ~GraphicsCard_Obsolete() = default;

		void Initialize(IDXGIFactory4* dxgiFactory, bool compute = false, bool copy = false);
		const ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		ID3D12CommandQueue** GetCommandQueueAddress(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		const ID3D12Device* GetDevice() const { return m_device_cp.Get(); }
		ID3D12Device* GetDevice() { return m_device_cp.Get(); }
		uint32_t GetNodeNum() { return m_node; }
		void ExecuteSync(_In_ uint32_t num, _In_reads_(num) ID3D12CommandList* const* ppCommandLists, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		void IsStable(bool isStable) { m_stableFlag = isStable; }

		//void GpuWait(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		//void GpuSignal(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		HRESULT GetDeviceRemovedReason() { return m_device_cp->GetDeviceRemovedReason(); }
	};

	class GraphicsCard : public NonCopyable
	{
		using ComCommandQueue = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;

		Microsoft::WRL::ComPtr<ID3D12Device6> m_device_cp;
		std::vector<ComCommandQueue> m_commandQueues;
		std::string m_name;
		std::mutex m_execMutex;

		D3D12_FEATURE_DATA_D3D12_OPTIONS m_featureDataOptions;
		D3D_FEATURE_LEVEL m_d3dFeatureLevel;
		bool m_isTypedUAVLoadSupport_R11G11B10_FLOAT;
		bool m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		bool m_stableFlag;  // 决定GPU是否为稳定的，若为稳定的，则限制供电以避免超频或降频。
		uint32_t m_node;


		void CreateDevice(IDXGIFactory7* dxgiFactory);
		void CreateCommandQueue();
		void GetHardwareAdapter(_In_ IDXGIFactory7* pFactory, _Outptr_result_maybenull_ IDXGIAdapter4** ppAdapter);

	public:
		GraphicsCard(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1);
		~GraphicsCard() = default;

		void Initialize(IDXGIFactory7* dxgiFactory);
		const ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		ID3D12CommandQueue** GetCommandQueueAddress(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		inline const ID3D12Device* GetDevice() const { return m_device_cp.Get(); }
		uint32_t GetNodeNum() { return m_node; }

		void IsStable(bool isStable) { m_stableFlag = isStable; }

		//void GpuWait(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		//void GpuSignal(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		HRESULT GetDeviceRemovedReason() { return m_device_cp->GetDeviceRemovedReason(); }
	};
}

#endif // !__GRAPHICSCARD_H__
