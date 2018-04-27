#include"GraphicCard.h"
#include"DebugLog.h"
#include"DTimer.h"
using namespace std;
using namespace Microsoft::WRL;

namespace AnEngine::RenderCore
{
	void GraphicsCard::CreateDevice(IDXGIFactory4* dxgiFactory)
	{
		D3D_FEATURE_LEVEL featureLevel;

		//CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(Private::r_dxgiFactory_cp.GetAddressOf()));
		//if (dxgiFactory == nullptr)
		//{
		//	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
		//}

		ComPtr<IDXGIAdapter1> cp_hardwareAdapter;
		//GetHardwareAdapter(Private::r_dxgiFactory_cp.Get(), &cp_hardwareAdapter);
		GetHardwareAdapter(dxgiFactory, &cp_hardwareAdapter);
		D3D12CreateDevice(cp_hardwareAdapter.Get(), r_MinD3DFeatureLevel_const, IID_PPV_ARGS(&m_device_cp));

		if (m_device_cp.Get() == nullptr)
		{
			ComPtr<IDXGIAdapter> cp_warpAdapter;
			dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&cp_warpAdapter));
			D3D12CreateDevice(cp_warpAdapter.Get(), r_MinD3DFeatureLevel_const, IID_PPV_ARGS(&m_device_cp));
		}

		m_device_cp->SetStablePowerState(m_stableFlag);

#if defined(DEBUG) //|| defined(_DEBUG)
		ID3D12InfoQueue* p_compInfoQueue = nullptr;
		ThrowIfFailed(m_device_cp->QueryInterface(IID_PPV_ARGS(&p_compInfoQueue)));
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

		// 通过ID来避免个人消息。
		D3D12_MESSAGE_ID denyMessageIds[] =
		{
			// 当描述符表中有未初始化的描述符时，即使着色器不访问，也会发生这种情况。常见的做法是切换着色器的排列而不是改变太多的代码、重新安排资源。
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
			// 当着色器不输出渲染目标的所有颜色分量（例如仅将RGB写入R10G10B10A2缓冲区时）忽略Alpha时触发。
			D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,
			// 即使当着色器不访问缺少的描述符，也会在描述符表未绑定时触发。不同的着色器之间共享的根签名并不都需要相同类型的资源。
			D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

			(D3D12_MESSAGE_ID)1008,
		};
		D3D12_INFO_QUEUE_FILTER newFilter = {};
		newFilter.DenyList.NumSeverities = _countof(severities);
		newFilter.DenyList.pSeverityList = severities;
		newFilter.DenyList.NumIDs = _countof(denyMessageIds);
		newFilter.DenyList.pIDList = denyMessageIds;

		p_compInfoQueue->PushStorageFilter(&newFilter);
		p_compInfoQueue->Release();
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
				if (SUCCEEDED(m_device_cp->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))) && (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
				{
					m_isTypedUAVLoadSupport_R11G11B10_FLOAT = true;
				}
				formatSupport.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				if (SUCCEEDED(m_device_cp->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))) && (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
				{
					m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
				}
			}
		}
	}

	void GraphicsCard::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			m_renderCommandQueue.Initialize(m_device_cp.Get());
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:
		{
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		{
			//D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			//queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			//queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			//m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_computeCommandQueue.GetAddressOf()));
			m_computeCommandQueue.Initialize(m_device_cp.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			m_copyCommandQueue.Initialize(m_device_cp.Get(), D3D12_COMMAND_LIST_TYPE_COPY);
			break;
		}
		default:
			break;
		}
	}

	const ID3D12Device* GraphicsCard::GetDevice() const
	{
		return m_device_cp.Get();
	}

	ID3D12Device * GraphicsCard::GetDevice()
	{
		return m_device_cp.Get();
	}

	uint32_t GraphicsCard::GetNodeNum()
	{
		return m_node;
	}

	void GraphicsCard::ExecuteSync(uint32_t num, ID3D12CommandList * const * ppCommandLists, D3D12_COMMAND_LIST_TYPE type)
	{
		//lock_guard<std::mutex> lock(m_execMutex);
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			m_renderCommandQueue.GetCommandQueue()->ExecuteCommandLists(num, ppCommandLists);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			break;
		case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:
			break;
		case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS:
			break;
		default:
			break;
		}
	}

	void GraphicsCard::IsStable(bool isStable)
	{
		m_stableFlag = isStable;
	}

	void GraphicsCard::GpuWait(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type)
	{
		ID3D12CommandQueue* commandQueue = GetCommandQueue(type);
		commandQueue->Wait(fence, value);
	}

	void GraphicsCard::GpuSignal(ID3D12Fence* fence, uint64_t value, D3D12_COMMAND_LIST_TYPE type)
	{
		ID3D12CommandQueue* commandQueue = GetCommandQueue(type);
		commandQueue->Signal(fence, value);
	}

	HRESULT GraphicsCard::GetDeviceRemovedReason()
	{
		return m_device_cp->GetDeviceRemovedReason();
	}

	const ID3D12CommandQueue* GraphicsCard::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			return m_renderCommandQueue.GetCommandQueue();
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:
		{
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		{
			return m_computeCommandQueue.GetCommandQueue();
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			return m_copyCommandQueue.GetCommandQueue();
			break;
		}
		default:
			break;
		}
		return nullptr;
	}

	ID3D12CommandQueue * GraphicsCard::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			return m_renderCommandQueue.GetCommandQueue();
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:
		{
			return nullptr;
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		{
			return m_computeCommandQueue.GetCommandQueue();
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			return m_copyCommandQueue.GetCommandQueue();
			break;
		}
		default:
			break;
		}
		return nullptr;
	}

	GraphicsCard::GraphicsCard() :
		m_stableFlag(false), m_node(0)
	{
	}

	void GraphicsCard::Initialize(IDXGIFactory4* dxgiFactory, bool compute, bool copy)
	{
		CreateDevice(dxgiFactory);
		CreateCommandQueue();
		if (compute) CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		if (copy) CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		/*m_device_cp->CreateFence(DTimer::GetInstance()->GetTotalTicks(), D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_fence_cp));*/
	}
}
