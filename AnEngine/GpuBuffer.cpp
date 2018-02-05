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
}