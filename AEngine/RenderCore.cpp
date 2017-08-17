#include"RenderCore.h"
#include"Screen.h"

// 检验是否有HDR输出功能
#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

namespace RenderCore
{
	void GraphicCard::CreateDevice()
	{
		UINT dxgiFactoryFlags = 0;

		// 开启Debug模式
#if defined(DEBUG) || defined(_DEBUG)
		ComPtr<ID3D12Debug> d3dDebugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
		{
			d3dDebugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		//ComPtr<IDXGIFactory4> r_cp_dxgiFactory;
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(Private::r_cp_dxgiFactory.GetAddressOf()));

		ComPtr<IDXGIAdapter1> cp_hardwareAdapter;
		GetHardwareAdapter(Private::r_cp_dxgiFactory.Get(), &cp_hardwareAdapter);
		D3D12CreateDevice(cp_hardwareAdapter.Get(), RenderCore::r_cnt_MinD3DFeatureLevel, IID_PPV_ARGS(&m_cp_device));

		if (m_cp_device.Get() == nullptr)
		{
			ComPtr<IDXGIAdapter> cp_warpAdapter;
			Private::r_cp_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&cp_warpAdapter));
			D3D12CreateDevice(cp_warpAdapter.Get(), RenderCore::r_cnt_MinD3DFeatureLevel, IID_PPV_ARGS(&m_cp_device));
		}

		m_cp_device->SetStablePowerState(m_stableFlag);

#if defined(DEBUG) || defined(_DEBUG)
		ID3D12InfoQueue* p_compInfoQueue;
		m_cp_device->QueryInterface(IID_PPV_ARGS(&p_compInfoQueue));
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
		if (SUCCEEDED(m_cp_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &m_featureDataOptions, sizeof(m_featureDataOptions))))
		{
			if (m_featureDataOptions.TypedUAVLoadAdditionalFormats)
			{
				D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport =
				{
					DXGI_FORMAT_R11G11B10_FLOAT,
					D3D12_FORMAT_SUPPORT1_NONE,
					D3D12_FORMAT_SUPPORT2_NONE
				};
				if (SUCCEEDED(m_cp_device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))) && (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
				{
					m_isTypedUAVLoadSupport_R11G11B10_FLOAT = true;
				}
				formatSupport.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				if (SUCCEEDED(m_cp_device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))) && (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
				{
					m_isTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
				}
			}
		}
	}

	void GraphicCard::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			m_renderCommandQueue.Initialize(m_cp_device.Get());
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
			m_computeCommandQueue.Initialize(m_cp_device.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			m_copyCommandQueue.Initialize(m_cp_device.Get(), D3D12_COMMAND_LIST_TYPE_COPY);
			break;
		}
		default:
			break;
		}
	}

	const ID3D12Device2* GraphicCard::GetDevice() const
	{
		return m_cp_device.Get();
	}

	ID3D12Device2 * GraphicCard::GetDevice()
	{
		return m_cp_device.Get();
	}

	void GraphicCard::IsStable(bool isStable)
	{
		m_stableFlag = isStable;
	}

	const ID3D12CommandQueue* GraphicCard::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
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

	ID3D12CommandQueue * GraphicCard::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
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

	GraphicCard::GraphicCard() :
		m_stableFlag(false)
	{
	}

	GraphicCard::GraphicCard(const GraphicCard & graphicCard)
	{
	}

	void GraphicCard::Initialize(bool compute, bool copy)
	{
		CreateDevice();
		CreateCommandQueue();
		if (compute) CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		if (copy) CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	}
}

namespace RenderCore
{
	void CommandQueue::Initialize(ID3D12Device2* p_device, D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = type;
		p_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_cp_commandQueue.GetAddressOf()));
		m_type = type;
	}

	void CommandQueue::Release()
	{
		CloseHandle(m_fenceEvent);
	}

	const ID3D12CommandQueue* CommandQueue::GetCommandQueue() const
	{
		return m_cp_commandQueue.Get();
	}

	ID3D12CommandQueue * CommandQueue::GetCommandQueue()
	{
		return m_cp_commandQueue.Get();
	}

	D3D12_COMMAND_LIST_TYPE CommandQueue::GetType()
	{
		return m_type;
	}
}

namespace RenderCore
{
	thread r_renderMainThread;

	vector<GraphicCard> r_renderCore;
	ComPtr<IDXGISwapChain1> r_cp_swapChain = nullptr;
	Resource::ColorBuffer r_displayPlane[r_cnt_SwapChainBufferCount];
	bool r_enableHDROutput = false;
#ifdef _WIN32
	HWND r_hwnd;
#endif // _WIN32

	namespace Private
	{
		ComPtr<IDXGIFactory4> r_cp_dxgiFactory;
	}

	void InitializeRender(int graphicCardCount, bool isStable, HWND hwnd)
	{
		r_hwnd = hwnd;
		while (graphicCardCount--)
		{
			GraphicCard aRender;
			aRender.IsStable(isStable);
			aRender.Initialize();
			r_renderCore.push_back(aRender);
		}
		InitializeSwapChain(Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), r_hwnd);
	}

	void InitializeSwapChain(int width, int height, HWND hwnd, DXGI_FORMAT dxgiFormat)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = r_cnt_SwapChainBufferCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = dxgiFormat;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;

		ComPtr<IDXGISwapChain1> swapChain1;
		//Private::r_cp_dxgiFactory->Create
		ThrowIfFailed(Private::r_cp_dxgiFactory->CreateSwapChainForHwnd
		(
			const_cast<ID3D12CommandQueue*>(r_renderCore[0].GetCommandQueue()),
			hwnd, &swapChainDesc, nullptr, nullptr, swapChain1.GetAddressOf()
		));
		swapChain1.As(&r_cp_swapChain);
#if CONDITIONALLY_ENABLE_HDR_OUTPUT && defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
		{
			IDXGISwapChain4* p_swapChain = static_cast<IDXGISwapChain4*>(r_cp_swapChain.Get());
			ComPtr<IDXGIOutput> cp_output;
			ComPtr<IDXGIOutput6> cp_output6;
			DXGI_OUTPUT_DESC1 outputDesc;
			UINT colorSpaceSupport;

			if (SUCCEEDED(p_swapChain->GetContainingOutput(&cp_output)) &&
				SUCCEEDED(cp_output.As(&cp_output6)) &&
				SUCCEEDED(cp_output6->GetDesc1(&outputDesc)) &&
				outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 &&
				SUCCEEDED(p_swapChain->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020, &colorSpaceSupport)) &&
				(colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) &&
				SUCCEEDED(p_swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)))
			{
				r_enableHDROutput = true;
			}
		}
#endif
		for (UINT i = 0; i < r_cnt_SwapChainBufferCount; ++i)
		{
			ComPtr<ID3D12Resource> cp_displayPlane;
			ThrowIfFailed(r_cp_swapChain->GetBuffer(i, IID_PPV_ARGS(&cp_displayPlane)));
			r_displayPlane[i].CreateFromSwapChain(L"Primary SwapChain Buffer",
				cp_displayPlane.Detach(), r_renderCore[0].GetDevice(),
				&RenderCore::Heap::r_h_heapDescAllocator);
		}
	}
}