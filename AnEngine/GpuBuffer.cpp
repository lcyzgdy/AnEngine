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
		desc.Width = (UINT64)m_bufferSize;
		return desc;
	}
	GpuBuffer::GpuBuffer() : m_bufferSize(0), m_elementCount(0), m_elementSize(0)
	{
		m_resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		m_uav.ptr = GpuVirtualAddressUnknown;
		m_srv.ptr = GpuVirtualAddressUnknown;
	}

	GpuBuffer::GpuBuffer(const std::wstring& name, uint32_t numElements, uint32_t elementSize, const void * initialData)
	{
		var device = r_graphicsCard[0]->GetDevice();

		m_elementCount = numElements;
		m_elementSize = elementSize;
		m_bufferSize = numElements * elementSize;

		D3D12_RESOURCE_DESC ResourceDesc = DescribeBuffer();

		m_usageState = D3D12_RESOURCE_STATE_COMMON;

		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		//device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE,
		//	&ResourceDesc, m_usageState, nullptr, IID_PPV_ARGS(&m_resource_cp));
		D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = device->GetResourceAllocationInfo(1, 1, &desc);
		D3D12_HEAP_DESC heapDesc;
		heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
		heapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDesc.Properties.CreationNodeMask = r_graphicsCard[0]->GetNodeNum();
		heapDesc.Properties.VisibleNodeMask = r_graphicsCard[0]->GetNodeNum();
		heapDesc.Alignment = allocationInfo.Alignment;
		heapDesc.Flags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
		device->CreateHeap(&heapDesc, IID_PPV_ARGS(&m_heap_cp));

		device->CreatePlacedResource(m_heap_cp.Get(), 0, &ResourceDesc, m_usageState, );

		m_gpuVirtualAddress = m_resource_cp->GetGPUVirtualAddress();


#ifdef RELEASE
		(name);
#else
		m_resource_cp->SetName(name.c_str());
#endif

		CreateDerivedViews();
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

	D3D12_CPU_DESCRIPTOR_HANDLE GpuBuffer::CreateConstantBufferView(uint32_t Offset, uint32_t Size) const
	{
		var device = r_graphicsCard[0]->GetDevice();
		Size = DMath::AlignUp(Size, 16);

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		CBVDesc.BufferLocation = m_gpuVirtualAddress + (size_t)Offset;
		CBVDesc.SizeInBytes = Size;

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
		size_t Offset = baseVertexIndex * m_elementSize;
		return VertexBufferView(Offset, (uint32_t)(m_bufferSize - Offset), m_elementSize);
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
}