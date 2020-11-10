#pragma once
#ifndef __GRAPHICSCARD_H__
#define __GRAPHICSCARD_H__
#include <mutex>

#include "onwind.h"
#include "DX.h"
#include "CommandQueue.h"

namespace AnEngine::RenderCore
{
	class GraphicsCard : public NonCopyable
	{
		using ComCommandQueue = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;

		Microsoft::WRL::ComPtr<ID3D12Device4> m_device_cp;
		std::vector<ComCommandQueue> m_commandQueues;
		std::string m_name;
		std::mutex m_execMutex;

		D3D12_FEATURE_DATA_D3D12_OPTIONS m_featureDataOptions;
		D3D_FEATURE_LEVEL m_d3dFeatureLevel;
		bool m_isTypedUAVLoadSupport_R11G11B10_FLOAT;
		bool m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		bool m_stableFlag;  // 决定GPU是否为稳定的，若为稳定的，则限制供电以避免超频或降频。
		uint32_t m_node;


		void CreateDevice(IDXGIFactory6* dxgiFactory);
		void CreateCommandQueue();
		void GetHardwareAdapter(_In_ IDXGIFactory6* pFactory, _Outptr_result_maybenull_ IDXGIAdapter4** ppAdapter);

	public:
		GraphicsCard(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1);
		GraphicsCard(GraphicsCard&& rhs);
		~GraphicsCard() = default;

		void Initialize(IDXGIFactory6* dxgiFactory);
		ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		ID3D12CommandQueue* const * GetCommandQueueAddress(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		inline ID3D12Device4* GetDevice() const { return m_device_cp.Get(); }
		uint32_t GetNodeNum() const { return m_node; }

		void IsStable(bool isStable) { m_stableFlag = isStable; }

		HRESULT GetDeviceRemovedReason() { return m_device_cp->GetDeviceRemovedReason(); }

		inline operator ID3D12Device* () const { return m_device_cp.Get(); }
	};
}

#endif // !__GRAPHICSCARD_H__
