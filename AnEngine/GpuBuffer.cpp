#include "GpuBuffer.h"
#include "RenderCore.h"
#include"DescriptorHeap.hpp"
#include"DMath.hpp"
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Heap;

namespace AnEngine::RenderCore::Resource
{
	D3D12_RESOURCE_DESC GpuBuffer::DescribeBuffer(void)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Alignment = 0;
		desc.DepthOrArraySize = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = m_resourceFlags;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Height = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Width = (uint64_t)m_bufferSize;
		return desc;
	}

	GpuBuffer::GpuBuffer(const std::wstring& name, uint32_t numElements, uint32_t elementSize,
		const void* initialData) : m_bufferSize(numElements * elementSize),
		m_elementCount(numElements), m_elementSize(elementSize)
	{
		m_resourceFlags = D3D12_RESOURCE_FLAG_NONE;
		m_uav.ptr = S_GpuVirtualAddressUnknown;
		m_srv.ptr = S_GpuVirtualAddressUnknown;

		var device = r_graphicsCard[0]->GetDevice();
		//D3D12_RESOURCE_DESC ResourceDesc = DescribeBuffer();
		//m_usageState = D3D12_RESOURCE_STATE_COMMON;
		m_state = D3D12_RESOURCE_STATE_GENERIC_READ;

		/*D3D12_HEAP_PROPERTIES heapProps;
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;*/

		//device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
		//	&ResourceDesc, m_usageState, nullptr, IID_PPV_ARGS(&m_resource_cp));
		var desc = DescribeBuffer();
		D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = device->GetResourceAllocationInfo(1, 1, &desc);

		D3D12_HEAP_DESC heapDesc;
		heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
		heapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDesc.Properties.CreationNodeMask = r_graphicsCard[0]->GetNodeNum();
		heapDesc.Properties.VisibleNodeMask = r_graphicsCard[0]->GetNodeNum();
		//heapDesc.Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		heapDesc.Alignment = 0;
		heapDesc.Flags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;

		//heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		/*ThrowIfFailed(device->CreateHeap(&heapDesc, IID_PPV_ARGS(&m_heap_cp)));

		ThrowIfFailed(device->CreatePlacedResource(m_heap_cp.Get(), 0, &desc, m_usageState,
			nullptr, IID_PPV_ARGS(&m_resource_cp)));*/

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_resource_cp)));

		m_gpuVirtualAddress = m_resource_cp->GetGPUVirtualAddress();

		if (initialData != nullptr)
		{
			std::byte* pVertexDataBegin;
			CD3DX12_RANGE readRange(0, 0);		//We do not intend to read from this resource on the CPU.
			m_resource_cp->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
			memcpy(pVertexDataBegin, initialData, m_bufferSize);
			m_resource_cp->Unmap(0, nullptr);
		}
#if defined _DEBUG || defined DEBUG
		m_resource_cp->SetName(name.c_str());
#else
		(name);
#endif

		//CreateDerivedViews();
	}

	GpuBuffer::~GpuBuffer()
	{
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE & GpuBuffer::GetUav(void) const
	{
		return m_uav;
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE & GpuBuffer::GetSrv(void) const
	{
		return m_srv;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GpuBuffer::RootConstantBufferView(void) const
	{
		return m_gpuVirtualAddress;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GpuBuffer::CreateConstantBufferView(uint32_t offset, uint32_t size) const
	{
		var device = r_graphicsCard[0]->GetDevice();
		size = DMath::AlignUp(size, 16);

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		CBVDesc.BufferLocation = m_gpuVirtualAddress + (size_t)offset;
		CBVDesc.SizeInBytes = size;

		D3D12_CPU_DESCRIPTOR_HANDLE hCbv = DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CreateConstantBufferView(&CBVDesc, hCbv);
		return hCbv;
	}

	D3D12_VERTEX_BUFFER_VIEW GpuBuffer::VertexBufferView(size_t offset, uint32_t size, uint32_t stride) const
	{
		D3D12_VERTEX_BUFFER_VIEW vbView;
		vbView.BufferLocation = m_gpuVirtualAddress + offset;
		vbView.SizeInBytes = size;
		vbView.StrideInBytes = stride;
		return vbView;
	}

	D3D12_VERTEX_BUFFER_VIEW GpuBuffer::VertexBufferView(size_t baseVertexIndex) const
	{
		size_t offset = baseVertexIndex * m_elementSize;
		return VertexBufferView(offset, (uint32_t)(m_bufferSize - offset), m_elementSize);
	}

	D3D12_INDEX_BUFFER_VIEW GpuBuffer::IndexBufferView(size_t offset, uint32_t size, bool b32Bit) const
	{
		D3D12_INDEX_BUFFER_VIEW ibView;
		ibView.BufferLocation = m_gpuVirtualAddress + offset;
		ibView.Format = b32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = size;
		return ibView;
	}

	D3D12_INDEX_BUFFER_VIEW GpuBuffer::IndexBufferView(size_t startIndex) const
	{
		size_t offset = startIndex * m_elementSize;
		return IndexBufferView(offset, (uint32_t)(m_bufferSize - offset), m_elementSize == 4);
	}

	GpuUploadBuffer::~GpuUploadBuffer()
	{
		if (m_resource.Get())
		{
			m_resource->Unmap(0, nullptr);
		}
	}

	void GpuUploadBuffer::Allocate(ID3D12Device* device, uint32_t bufferSize, wchar_t * resourceName)
	{
		var uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		var bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
		ThrowIfFailed(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_resource)));
		m_resource->SetName(resourceName);
	}

	uint8_t* GpuUploadBuffer::MapCpuWriteOnly()
	{
		uint8_t* mappedData;
		// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));
		return mappedData;
	}
}

namespace AnEngine::RenderCore::Resource
{
	ByteAddressBuffer::ByteAddressBuffer(const std::wstring & name, uint32_t numElements, uint32_t elementSize,
		const void* initialData) : GpuBuffer(name, numElements, elementSize, initialData)
	{
		//CreateDerivedViews();
	}

	void ByteAddressBuffer::CreateDerivedViews()
	{
		var device = r_graphicsCard[0]->GetDevice();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = (UINT)m_bufferSize / 4;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		if (m_srv.ptr == S_GpuVirtualAddressUnknown)
		{
			m_srv = DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		device->CreateShaderResourceView(m_resource_cp.Get(), &srvDesc, m_uav);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.NumElements = (UINT)m_bufferSize / 4;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		if (m_uav.ptr == S_GpuVirtualAddressUnknown)
			m_uav = DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CreateUnorderedAccessView(m_resource_cp.Get(), nullptr, &uavDesc, m_uav);
	}
}