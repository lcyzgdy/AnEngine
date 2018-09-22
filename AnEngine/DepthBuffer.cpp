#include "DepthBuffer.h"
#include "RenderCore.h"

namespace AnEngine::RenderCore::Resource
{
	/*void DepthBuffer::CreateDerviedViews(ID3D12Device* device, DXGI_FORMAT & format,
		Heap::DescriptorHeapAllocator* descAllocator)
	{
		ID3D12Resource* resource = m_resource_cp.Get();

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

		if (m_dsvHandle[0].ptr == Resource::S_GpuVirtualAddressUnknown)
		{
			m_dsvHandle[0] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			m_dsvHandle[1] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		}
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		device->CreateDepthStencilView(resource, &dsvDesc, m_dsvHandle[0]);
		dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		device->CreateDepthStencilView(resource, &dsvDesc, m_dsvHandle[1]);

		DXGI_FORMAT stencilReadFormat = GetStencilFormat(format);
		if (stencilReadFormat != DXGI_FORMAT_UNKNOWN)
		{
			if (m_dsvHandle[2].ptr == Resource::S_GpuVirtualAddressUnknown)
			{
				m_dsvHandle[2] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
				m_dsvHandle[3] = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
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

		if (m_depthSrvHandle.ptr == Resource::S_GpuVirtualAddressUnknown)
		{
			m_depthSrvHandle = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
			if (m_stencilSrvHandle.ptr == Resource::S_GpuVirtualAddressUnknown)
			{
				m_stencilSrvHandle = descAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
			srvDesc.Format = stencilReadFormat;
			device->CreateShaderResourceView(resource, &srvDesc, m_depthSrvHandle);
		}
	}*/

	D3D12_RESOURCE_DESC DepthBuffer::DescribeAsDepthBuffer()
	{
		D3D12_RESOURCE_DESC desc;
		desc.Width = m_width;
		desc.Height = m_height;
		desc.Format = m_format;
		desc.DepthOrArraySize = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		return desc;
	}

	DepthBuffer::DepthBuffer(uint32_t width, uint32_t height, float clearDepth) :
		PixelBuffer(width, height, 1, DXGI_FORMAT_R8G8B8A8_UNORM), m_clearDepth(clearDepth)
	{
		var device = r_graphicsCard[0]->GetDevice();
		device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER,
			&DescribeAsDepthBuffer(), D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_resource_cp));
	}

	/*void DepthBuffer::Create(const wstring& name, uint32_t _width, uint32_t _height,
		DXGI_FORMAT& format, Heap::DescriptorHeapAllocator* descAllocator,
		uint32_t numSamples, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr)
	{
		ID3D12Device* device = r_graphicsCard[0]->GetDevice();

		D3D12_RESOURCE_DESC desc = DescribeTex2D(_width, _height, 1, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		desc.SampleDesc.Count = numSamples;
		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		CreateTextureResource(device, name, desc, clearValue, vidMemPtr);
		CreateDerviedViews(device, format, descAllocator);
	}*/
}