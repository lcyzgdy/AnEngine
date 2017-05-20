#include"RenderCore.h"

namespace RenderCore
{
	vector<GraphicCard> r_renderCore;
	ComPtr<IDXGISwapChain1> r_swapChain = nullptr;

	void GraphicCard::CreateDevice()
	{
		UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		ComPtr<ID3D12Debug> d3dDebugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController)))
		{
			d3dDebugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		// ¿ªÆôDebugÄ£Ê½

		ComPtr<IDXGIFactory4> dxgiFactory;
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
		if (IsUseWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
			D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(dxgiFactory.Get(), &hardwareAdapter);
			D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		}

		if (device.Get() == nullptr)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
			D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		}
	}

	void GraphicCard::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			renderCommandQueue.Initialize(device.Get());
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
			//device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(computeCommandQueue.GetAddressOf()));
			computeCommandQueue.Initialize(device.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			/*D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(copyCommandQueue.GetAddressOf()));*/
			copyCommandQueue.Initialize(device.Get(), D3D12_COMMAND_LIST_TYPE_COPY);
			break;
		}
		default:
			break;
		}
	}

	const ID3D12Device* GraphicCard::GetDevice() const
	{
		return device.Get();
	}

	const ID3D12CommandQueue* GraphicCard::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			return renderCommandQueue.GetCommandQueue();
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:
		{
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		{
			return computeCommandQueue.GetCommandQueue();
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			return copyCommandQueue.GetCommandQueue();
			break;
		}
		default:
			break;
		}
		return nullptr;
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

	void InitializeRender(int graphicCardCount)
	{
		while (graphicCardCount--)
		{
			GraphicCard aRender;
			aRender.Initialize();
			r_renderCore.push_back(aRender);
		}
	}

	void CommandQueue::Initialize(const ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
	{
	}

	void CommandQueue::Release()
	{
	}

	const ID3D12CommandQueue* CommandQueue::GetCommandQueue() const
	{
		return nullptr;
	}

	D3D12_COMMAND_LIST_TYPE CommandQueue::GetType()
	{
		return D3D12_COMMAND_LIST_TYPE();
	}
}