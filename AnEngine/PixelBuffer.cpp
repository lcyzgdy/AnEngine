#include "PixelBuffer.h"
#include "RenderCore.h"
#include "DescriptorHeap.hpp"
using namespace std;
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Heap;

namespace AnEngine::RenderCore::Resource
{
	PixelBuffer::PixelBuffer(uint32_t width, uint32_t height, uint32_t depthOrArraySize, DXGI_FORMAT format) :
		m_width(width), m_height(height), m_size(depthOrArraySize), m_format(format)
	{
	}

	PixelBuffer::PixelBuffer(const D3D12_RESOURCE_DESC& desc) : m_width(desc.Width), m_height(desc.Height),
		m_size(desc.DepthOrArraySize), m_format(desc.Format)
	{
	}

	PixelBuffer::PixelBuffer(D3D12_RESOURCE_DESC&& desc) : m_width(desc.Width), m_height(desc.Height),
		m_size(desc.DepthOrArraySize), m_format(desc.Format)
	{
	}

	void PixelBuffer::SetAsRtv()
	{
		ID3D12Device* device = r_graphicsCard[0]->GetDevice();
		var handle = Heap::DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		device->CreateRenderTargetView(m_resource_cp.Get(), nullptr, handle);
		m_rtvHandle = handle;
	}

	void PixelBuffer::SetAsDsv()
	{
		var device = r_graphicsCard[0]->GetDevice();
		var handle = Heap::DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		device->CreateDepthStencilView(m_resource_cp.Get(), nullptr, handle);
		m_dsvHandle = handle;
	}

	void PixelBuffer::SetAsSrv()
	{
		var device = r_graphicsCard[0]->GetDevice();
		var handle = Heap::DescriptorHeapAllocator::GetInstance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CreateShaderResourceView(m_resource_cp.Get(), nullptr, handle);
		m_srvHandle = handle;
	}

	/*void PixelBuffer::ExportToFile(wstring & filePath, ID3D12Device* device)
	{
		// 创建一个足够大的纹理缓冲区
		D3D12_HEAP_PROPERTIES heapProps;
		heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		// 缓冲区必须是1维的，对于Texture2D要进行降维。
		D3D12_RESOURCE_DESC tempDesc = {};
		tempDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		tempDesc.Width = m_width * m_height * BytesPerPixel(m_format);
		tempDesc.Height = 1;
		tempDesc.DepthOrArraySize = 1;
		tempDesc.MipLevels = 1;
		tempDesc.Format = DXGI_FORMAT_UNKNOWN;
		tempDesc.SampleDesc.Count = 1;
		tempDesc.SampleDesc.Quality = 0;
		tempDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		tempDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// 创建缓冲
		ID3D12Resource* p_tempResource;
		ThrowIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &tempDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&p_tempResource)));
		p_tempResource->SetName(L"Temporary Readback Buffer");
		GpuResource tempResource(p_tempResource, D3D12_RESOURCE_STATE_COPY_DEST);

		///CommandContext::ReadbackTexture2D(tempResource, *this);

		// 指针指向缓冲区，指针是CPU可读的。
		void* p_memory;
		p_tempResource->Map(0, &CD3DX12_RANGE(0, tempDesc.Width), &p_memory);

		// 打开文件，写入标题，再写入纹理数据。
		ofstream outFile(filePath, std::ios::out | std::ios::binary);
		outFile.write((const char*)&m_format, 4);
		outFile.write((const char*)&m_width, 4); // Pitch
		outFile.write((const char*)&m_width, 4);
		outFile.write((const char*)&m_height, 4);
		outFile.write((const char*)p_memory, tempDesc.Width);
		outFile.close();

		// 在取消映射的时候使用空范围。
		p_tempResource->Unmap(0, &CD3DX12_RANGE(0, 0));
		p_tempResource->Release();
	}*/

	DXGI_FORMAT PixelBuffer::GetBaseFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_TYPELESS;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_TYPELESS;

			// 32位Z模板
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32G8X24_TYPELESS;

			// 没有模板
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_TYPELESS;

			// 24位Z模板
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24G8_TYPELESS;

			// 16位Z模板
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_TYPELESS;

		default:
			return format;
		}
	}

	DXGI_FORMAT PixelBuffer::GetUAVFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;

		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM;

		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

#if defined(DEBUG) || defined(_DEBUG)
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_D16_UNORM:
			throw exception();
#endif
		default:
			return format;
		}
	}

	DXGI_FORMAT PixelBuffer::GetDSVFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
			// 32
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

			// 无
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;

			// 24
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

			// 16位Z模板
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_D16_UNORM;

		default:
			return format;
		}
	}

	DXGI_FORMAT PixelBuffer::GetDepthFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
			// 32
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

			// No
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

			// 24
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

			// 16
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;

		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT PixelBuffer::GetStencilFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
			// 32
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

			// 24
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	size_t PixelBuffer::BytesPerPixel(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 16;

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 12;

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return 8;

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return 4;

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_A8P8:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			return 2;

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
		case DXGI_FORMAT_P8:
			return 1;

		default:
			return 0;
		}
	}

	D3D12_RESOURCE_DESC PixelBuffer::DescribeTex2D(uint32_t width, uint32_t height, uint32_t depthOrArraySize, uint32_t numMips,
		DXGI_FORMAT format, uint32_t flags)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Alignment = 0;
		desc.DepthOrArraySize = static_cast<uint16_t>(depthOrArraySize);
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(flags);
		desc.Format = GetBaseFormat(format);
		desc.Height = static_cast<uint32_t>(height);
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = static_cast<uint16_t>(numMips);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Width = static_cast<uint64_t>(width);
		return desc;
	}

	D3D12_RESOURCE_DESC PixelBuffer::DescribeMsaaTex2D(uint32_t width, uint32_t height, uint32_t depthOrArraySize, uint32_t numMips,
		DXGI_FORMAT format, uint32_t flags, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS& msaaQl)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Alignment = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.DepthOrArraySize = static_cast<uint16_t>(depthOrArraySize);
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(flags);
		//desc.Format = GetBaseFormat(format);
		desc.Format = format;
		desc.Height = static_cast<uint32_t>(height);
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = static_cast<uint16_t>(numMips);
		desc.SampleDesc.Count = msaaQl.SampleCount;
		desc.SampleDesc.Quality = msaaQl.NumQualityLevels - 1;
		desc.Width = static_cast<uint64_t>(width);
		return desc;
	}

	void PixelBuffer::AssociateWithResource(ID3D12Device * device, const wstring & name, ID3D12Resource* resource,
		D3D12_RESOURCE_STATES currentState)
	{
		(device); // 支持多适配器时要改正!!!

		if (resource == nullptr) throw exception();
		D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();

		m_resource_cp.Attach(resource);
		m_state = currentState;

		m_width = (uint32_t)resourceDesc.Width;		// 暂时不关心过大的地址。
		m_height = resourceDesc.Height;
		m_size = resourceDesc.DepthOrArraySize;
		m_format = resourceDesc.Format;

#if defined _DEBUG || defined DEBUG
		m_resource_cp->SetName(name.c_str());
#endif
	}

	void PixelBuffer::CreateTextureResource(ID3D12Device* device, const wstring & name,
		const D3D12_RESOURCE_DESC & resourceDesc, D3D12_CLEAR_VALUE clearValue,
		D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr)
	{
		//this->Release();

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(m_resource_cp.GetAddressOf())));

		m_state = D3D12_RESOURCE_STATE_COMMON;
		m_gpuVirtualAddress = Resource::S_GpuVirtualAddressNull;

#ifndef RELEASE
		m_resource_cp->SetName(name.c_str());
#endif
	}

	void PixelBuffer::CreateTextureResource(ID3D12Device* device, const wstring& name, const D3D12_RESOURCE_DESC& resourceDesc,
		D3D12_CLEAR_VALUE clearValue)
	{
		CreateTextureResource(device, name, resourceDesc, clearValue, Resource::S_GpuVirtualAddressUnknown);
	}
	}