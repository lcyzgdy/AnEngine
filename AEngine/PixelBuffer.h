#pragma once
#ifndef __PIXELBUFFER_H__
#define __PIXELBUFFER_H__
#include"DX.h"
#include"GpuResource.h"
#include"RenderCore.h"
using namespace std;
using namespace RenderCore;
using namespace RenderCore::Resource;

namespace RenderCore
{
	namespace Resource
	{
		class PixelBuffer :public GpuResource
		{
		protected:
			UINT m_width;
			UINT m_height;
			UINT m_size;
			UINT m_bankRotation;
			DXGI_FORMAT m_format;

			DXGI_FORMAT GetBaseFormat(DXGI_FORMAT& format);
			DXGI_FORMAT GetUAVFormat(DXGI_FORMAT& format);
			DXGI_FORMAT GetDSVFormat(DXGI_FORMAT& format);
			DXGI_FORMAT GetDepthFormat(DXGI_FORMAT& format);
			DXGI_FORMAT GetStencilFormat(DXGI_FORMAT& format);
			size_t BytesPerPixel(DXGI_FORMAT& format);

			D3D12_RESOURCE_DESC DescribeTex2D(UINT width, UINT height, UINT depthOrArraySize, 
				UINT numMips, DXGI_FORMAT format, UINT flags);

			void AssociateWithResource(ID3D12Device* p_device, const wstring& name, 
				ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState);

			void CreateTextureResource(ID3D12Device* p_device, const wstring& name,
				const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue,
				D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = Resource::GpuVirtualAddressUnknown);

			void CreateTextureResource(ID3D12Device* p_device, const wstring& name,
				const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue/*,EsramAllocator& allocator*/);

		public:
			PixelBuffer();
			~PixelBuffer() = default;

			UINT GetWidth();
			UINT GetHeight();
			UINT GetDepth();
			const DXGI_FORMAT& GetFormat() const;

			void SetBankRotation(UINT rotationAmount);

			void ExportToFile(wstring& filePath);
		};
	}
}

#endif // !__PIXELBUFFER_H__
