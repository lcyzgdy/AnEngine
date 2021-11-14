#include"GraphicsCard.h"
#include"DebugLog.h"
#include"DTimer.h"
#include"RenderCore.h"
using namespace std;
using namespace Microsoft::WRL;

namespace AnEngine::RenderCore
{
	void GraphicsCard::GetHardwareAdapter(_In_ IDXGIFactory6* pFactory, _Outptr_result_maybenull_ IDXGIAdapter4** ppAdapter)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;
		*ppAdapter = nullptr;

		for (uint32_t adapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf()));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC3 desc;
			adapter->GetDesc3(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// 不选择基本显示适配器，如果想使用软件渲染，在命令行中加入"/warp"
				continue;
			}

			// 检查显卡是否支持DX 12，但是不创建真实的显卡
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
		*ppAdapter = adapter.Detach();
	}

	void GraphicsCard::CreateDevice(IDXGIFactory6* dxgiFactory)
	{
		D3D_FEATURE_LEVEL featureLevel;

		ComPtr<IDXGIAdapter4> cp_hardwareAdapter;
		GetHardwareAdapter(dxgiFactory, cp_hardwareAdapter.GetAddressOf());
		ThrowIfFailed(D3D12CreateDevice(cp_hardwareAdapter.Get(), m_d3dFeatureLevel, IID_PPV_ARGS(m_device_cp.GetAddressOf())), R_GetGpuError);

		if (m_device_cp.Get() == nullptr)
		{
			ComPtr<IDXGIAdapter> cp_warpAdapter;
			dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(cp_warpAdapter.GetAddressOf()));
			ThrowIfFailed(D3D12CreateDevice(cp_warpAdapter.Get(), m_d3dFeatureLevel, IID_PPV_ARGS(m_device_cp.GetAddressOf())));
		}

		m_device_cp->SetStablePowerState(m_stableFlag);

#ifndef NDEBUG
		// Configure debug device (if active).
		ComPtr<ID3D12InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(m_device_cp.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D12_MESSAGE_ID hide[] =
			{
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
			};
			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}

#endif

		if (SUCCEEDED(m_device_cp->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &m_featureDataOptions, sizeof(m_featureDataOptions))))
		{
			if (m_featureDataOptions.TypedUAVLoadAdditionalFormats)
			{
				D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport =
				{
					DXGI_FORMAT_R11G11B10_FLOAT,
					D3D12_FORMAT_SUPPORT1_NONE,
					D3D12_FORMAT_SUPPORT2_NONE
				};
				if (SUCCEEDED(m_device_cp->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport,
					sizeof(formatSupport))) && (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
				{
					m_isTypedUAVLoadSupport_R11G11B10_FLOAT = true;
				}
				formatSupport.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				if (SUCCEEDED(m_device_cp->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport,
					sizeof(formatSupport))) && (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
				{
					m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
				}
			}
		}
	}

	void GraphicsCard::CreateCommandQueue()
	{
		令(device = m_device_cp.Get());
		for (int i = 0; i < 6; i++)
		{
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = (D3D12_COMMAND_LIST_TYPE)i;
			device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueues[i].GetAddressOf()));
		}
	}

	GraphicsCard::GraphicsCard(D3D_FEATURE_LEVEL featureLevel) :m_commandQueues(0), m_node(0), m_d3dFeatureLevel(featureLevel),
		m_isTypedUAVLoadSupport_R11G11B10_FLOAT(false), m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT(false), m_stableFlag(false)
	{

	}

	GraphicsCard::GraphicsCard(GraphicsCard&& rhs) : m_device_cp(rhs.m_device_cp), m_commandQueues(rhs.m_commandQueues),
		m_name(rhs.m_name), m_node(rhs.m_node), m_d3dFeatureLevel(rhs.m_d3dFeatureLevel),
		m_isTypedUAVLoadSupport_R11G11B10_FLOAT(rhs.m_isTypedUAVLoadSupport_R11G11B10_FLOAT),
		m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT(rhs.m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT),
		m_stableFlag(rhs.m_stableFlag)
	{
	}

	void GraphicsCard::Initialize(IDXGIFactory6* dxgiFactory)
	{
		CreateDevice(dxgiFactory);
		CreateCommandQueue();
	}

	ID3D12CommandQueue* GraphicsCard::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
	{
		return m_commandQueues[static_cast<int>(type)].Get();
	}

	ID3D12CommandQueue* const* GraphicsCard::GetCommandQueueAddress(D3D12_COMMAND_LIST_TYPE type) const
	{
		return m_commandQueues[static_cast<int>(type)].GetAddressOf();
	}
}
