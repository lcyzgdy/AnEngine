#include "DepthBuffer.h"

void RenderCore::Resource::DepthBuffer::CreateDerviedViews(ID3D12Device * device, DXGI_FORMAT & format,
	RenderCore::Heap::DescriptorAllocator* descAllocator)
{
	ID3D12Resource* resource = m_cp_resource.Get();

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = format;
	if (resource->GetDesc().SampleDesc.Count == 1)
	{
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
	}
	else
	{
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	}

	if (m_dsvHandle[0].ptr == Resource::GpuVirtualAddressUnknown)
	{
		m_dsvHandle[0] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, device);
		m_dsvHandle[1] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, device);
	}
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	device->CreateDepthStencilView(resource, &dsvDesc, m_dsvHandle[0]);
	dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
	device->CreateDepthStencilView(resource, &dsvDesc, m_dsvHandle[1]);
	
	DXGI_FORMAT stencilReadFormat = GetStencilFormat(format);
	if (stencilReadFormat != DXGI_FORMAT_UNKNOWN)
	{
		if (m_dsvHandle[2].ptr == Resource::GpuVirtualAddressUnknown)
		{
			m_dsvHandle[2] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, device);
			m_dsvHandle[3] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, device);
		}

		dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
		device->CreateDepthStencilView(resource, &dsvDesc, m_dsvHandle[2]);
		dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL | D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		device->CreateDepthStencilView(resource, &dsvDesc, m_dsvHandle[3]);
	}
	else
	{
		m_dsvHandle[2] = m_dsvHandle[0];
		m_dsvHandle[1] = m_dsvHandle[1];
	}

	if (m_depthSrvHandle.ptr == Resource::GpuVirtualAddressUnknown)
	{
		m_depthSrvHandle = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, device);
	}
	// 创建SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = GetDepthFormat(format);
	if (dsvDesc.ViewDimension == D3D12_DSV_DIMENSION_TEXTURE2D)
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
	}
	else
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	}
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	device->CreateShaderResourceView(resource, &srvDesc, m_depthSrvHandle);

	if (stencilReadFormat != DXGI_FORMAT_UNKNOWN)
	{
		if (m_stencilSrvHandle.ptr == Resource::GpuVirtualAddressUnknown)
		{
			m_stencilSrvHandle = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, device);
		}
		srvDesc.Format = stencilReadFormat;
		device->CreateShaderResourceView(resource, &srvDesc, m_depthSrvHandle);
	}
}

RenderCore::Resource::DepthBuffer::DepthBuffer(float clearDepth, uint8_t clearStencil) :
	m_clearDepth(clearDepth), m_clearStencil(clearStencil)
{
	m_dsvHandle[0].ptr = Resource::GpuVirtualAddressUnknown;
	m_dsvHandle[1].ptr = Resource::GpuVirtualAddressUnknown;
	m_dsvHandle[2].ptr = Resource::GpuVirtualAddressUnknown;
	m_dsvHandle[3].ptr = Resource::GpuVirtualAddressUnknown;
	m_depthSrvHandle.ptr = Resource::GpuVirtualAddressUnknown;
	m_stencilSrvHandle.ptr = Resource::GpuVirtualAddressUnknown;
}

void RenderCore::Resource::DepthBuffer::Create(const wstring & name, uint32_t _width, uint32_t _height,
	DXGI_FORMAT & format, ID3D12Device* device, RenderCore::Heap::DescriptorAllocator* descAllocator,
	uint32_t numSamples, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr)
{
	D3D12_RESOURCE_DESC desc =
		DescribeTex2D(_width, _height, 1, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	desc.SampleDesc.Count = numSamples;
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	CreateTextureResource(device, name, desc, clearValue, vidMemPtr);
	CreateDerviedViews(device, format, descAllocator);
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE & RenderCore::Resource::DepthBuffer::GetDsv() const
{
	return m_dsvHandle[0];
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE & RenderCore::Resource::DepthBuffer::GetDsvDepthReadOnly() const
{
	return m_dsvHandle[1];
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE & RenderCore::Resource::DepthBuffer::GetDsvStencilReadOnly() const
{
	return m_dsvHandle[2];
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE & RenderCore::Resource::DepthBuffer::GetDsvReadOnly() const
{
	return m_dsvHandle[3];
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE & RenderCore::Resource::DepthBuffer::GetDepthSrv() const
{
	return m_depthSrvHandle;
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE & RenderCore::Resource::DepthBuffer::GetStencilSrv() const
{
	return m_stencilSrvHandle;
}

inline float RenderCore::Resource::DepthBuffer::GetClearDepth() const
{
	return m_clearDepth;
}

inline uint8_t RenderCore::Resource::DepthBuffer::GetClearStencil() const
{
	return m_clearStencil;
}
