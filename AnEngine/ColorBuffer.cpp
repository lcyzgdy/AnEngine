#include "ColorBuffer.h"
#include "DescriptorHeap.hpp"
#include "RenderCoreConstants.h"
#include "RenderCore.h"
using namespace std;
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::RenderCore::Resource
{
	/*ColorBuffer::ColorBuffer() :
		PixelBuffer(), m_clearColor(Color(0.0f, 0.0f, 0.0f)),
		m_numMipMaps(0), m_fragmentCount(1), m_sampleCount(1)
	{
	}*/

	/*ColorBuffer::ColorBuffer(const Color& clearColor) :
		m_clearColor(clearColor), m_numMipMaps(0), m_fragmentCount(1), m_sampleCount(1)
	{
		m_rtvHandle.ptr = S_GpuVirtualAddressUnknown;
		m_srvHandle.ptr = S_GpuVirtualAddressUnknown;
		memset(m_uavHandle, 0xff, sizeof(m_uavHandle));
	}*/

	ColorBuffer::ColorBuffer(const wstring& name, uint32_t width, uint32_t height, uint32_t numMips,
		DXGI_FORMAT format, D3D12_HEAP_TYPE heapType, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr) :
		PixelBuffer(width, height, 1, format), m_numMipMaps(numMips), m_fragmentCount(1)
	{
		GraphicsCard* device = r_graphicsCard[0].get();

		var desc = DescribeTex2D(width, height, 1, numMips, format, D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE);
		// 确定资源大小
		D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = device->GetDevice()->GetResourceAllocationInfo(1, 1, &desc);
		// 创建资源堆，现在使用默认堆
		/*D3D12_HEAP_DESC heapDesc;
		heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
		heapDesc.Properties.Type = heapType;
		heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDesc.Properties.CreationNodeMask = device->GetNodeNum();
		heapDesc.Properties.VisibleNodeMask = device->GetNodeNum();
		heapDesc.Alignment = allocationInfo.Alignment;
		heapDesc.Flags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;*/
		//ThrowIfFailed(device->GetDevice()->CreateHeap(&heapDesc, IID_PPV_ARGS(m_heap_cp.GetAddressOf())));

		/*ThrowIfFailed(device->GetDevice()->CreatePlacedResource(m_heap_cp.Get(), 0, &desc, D3D12_RESOURCE_STATE_RENDER_TARGET,
			nullptr, IID_PPV_ARGS(m_resource_cp.GetAddressOf())));*/
	}

	ColorBuffer::ColorBuffer(const wstring& name, uint32_t width, uint32_t height, uint32_t numMips,
		uint32_t msaaSampleCount, DXGI_FORMAT format, D3D12_HEAP_TYPE heapType, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr) :
		PixelBuffer(width, height, 1, DXGI_FORMAT_R8G8B8A8_UNORM), m_sampleCount(msaaSampleCount),
		m_numMipMaps(numMips), m_fragmentCount(1)
	{
		GraphicsCard* device = r_graphicsCard[0].get();

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQl;
		msaaQl.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		msaaQl.SampleCount = msaaSampleCount;
		msaaQl.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		ThrowIfFailed(device->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQl, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));

		var desc = DescribeMsaaTex2D(width, height, 1, numMips, format, D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE | 1, msaaQl);
		// 确定资源大小
		D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = device->GetDevice()->GetResourceAllocationInfo(0x1, 1, &desc);
		// 创建资源堆，现在使用默认堆
		/*D3D12_HEAP_DESC heapDesc;
		heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
		heapDesc.Properties.Type = heapType;
		heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDesc.Properties.CreationNodeMask = device->GetNodeNum();
		heapDesc.Properties.VisibleNodeMask = device->GetNodeNum();
		heapDesc.Alignment = allocationInfo.Alignment;
		heapDesc.Flags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
		ThrowIfFailed(device->GetDevice()->CreateHeap(&heapDesc, IID_PPV_ARGS(&m_heap_cp)));

		ThrowIfFailed(device->GetDevice()->CreatePlacedResource(m_heap_cp.Get(), 0, &desc, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_resource_cp)));*/
	}

	ColorBuffer::ColorBuffer(const D3D12_RESOURCE_DESC& desc) : PixelBuffer(desc)
	{
		ThrowIfFailed(r_graphicsCard[0]->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr,
			IID_PPV_ARGS(&m_resource_cp)));
	}

	ColorBuffer::ColorBuffer(D3D12_RESOURCE_DESC&& desc) : PixelBuffer(desc)
	{
		ThrowIfFailed(r_graphicsCard[0]->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr,
			IID_PPV_ARGS(&m_resource_cp)));
	}

	ColorBuffer::ColorBuffer(const wstring& name, ID3D12Resource* baseResource, D3D12_CPU_DESCRIPTOR_HANDLE handle) :
		PixelBuffer(baseResource->GetDesc())
	{
		var device = r_graphicsCard[0]->GetDevice();
		AssociateWithResource(device, name, baseResource, D3D12_RESOURCE_STATE_PRESENT);
		m_rtvHandle = handle;
		device->CreateRenderTargetView(m_resource_cp.Get(), nullptr, m_rtvHandle);
	}



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////



	D3D12_RESOURCE_DESC ColorBuffer::DescribeAsGBuffer(uint32_t width, uint32_t height)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.DepthOrArraySize = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Height = height;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Width = width;
		return desc;
	}
}