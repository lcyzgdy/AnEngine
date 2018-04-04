#pragma once
#ifndef __PIXELBUFFER_H__
#define __PIXELBUFFER_H__
#include"DX.h"
#include"GpuResource.h"
using namespace std;

namespace AnEngine::RenderCore::Resource
{
	class PixelBuffer : public GpuResource
	{
	protected:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
		uint32_t m_bankRotation;
		DXGI_FORMAT m_format;

		DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
		DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
		DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
		DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
		DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
		size_t BytesPerPixel(DXGI_FORMAT format);

		D3D12_RESOURCE_DESC DescribeTex2D(uint32_t width, uint32_t height, uint32_t depthOrArraySize,
			uint32_t numMips, DXGI_FORMAT format, uint32_t flags);

		D3D12_RESOURCE_DESC DescribeMsaaTex2D(uint32_t width, uint32_t height, uint32_t depthOrArraySize,
			uint32_t numMips, DXGI_FORMAT format, uint32_t flags, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS& msaaQl);

		void AssociateWithResource(ID3D12Device* device, const wstring& name,
			ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState);

		void CreateTextureResource(ID3D12Device* device, const wstring& name,
			const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue,
			D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = Resource::GpuVirtualAddressUnknown);

		void CreateTextureResource(ID3D12Device* device, const wstring& name,
			const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue/*,EsramAllocator& allocator*/);

	public:
		PixelBuffer();
		PixelBuffer(uint32_t width, uint32_t height, uint32_t depthOrArraySize, DXGI_FORMAT format);
		//virtual ~PixelBuffer() = default;

		uint32_t GetWidth();
		uint32_t GetHeight();
		uint32_t GetDepth();
		DXGI_FORMAT GetFormat();

		void SetBankRotation(uint32_t rotationAmount);

		void ExportToFile(wstring& filePath, ID3D12Device* device);
	};
}

#endif // !__PIXELBUFFER_H__
