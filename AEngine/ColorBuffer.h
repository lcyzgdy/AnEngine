#pragma once
#ifndef __COLORBUFFER_H__
#define __COLORBUFFER_H__

#include"DX.h"
#include"PixelBuffer.h"
#include"Color.h"
using namespace std;
using namespace RenderCore;

namespace RenderCore
{
	namespace Resource
	{
		class ColorBuffer: public PixelBuffer
		{
		protected:
			D3D12_RESOURCE_FLAGS CombineResourceFlags() const;

		public:
			ColorBuffer() = default;
			~ColorBuffer() = default;

			// 从交换链缓冲区创建颜色缓冲区，无序访问受限。
			void CreateFromSwapChain(const wstring& name, ID3D12Resource* baseResource);
			// 创建颜色缓冲区，如果提供了地址则不会分配内存。虚拟地址允许重命名缓冲器（对于跨越帧重用ESRAM特别有用）。？？
			void Create(const wstring& name, uint32_t _width, uint32_t _height, uint32_t numMips,
				DXGI_FORMAT& format, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = RenderCore::Resource::GpuVirtualAddressUnknown);
			// 创建颜色缓冲区，将内存分配到ESRAM（Xbox One）。在Windows上，这个功能与Create()没有视频地址时相同。 ??
			//void Create(const std::wstring& name, uint32_t _width, uint32_t _height, uint32_t numMips,
			//	DXGI_FORMAT& format, EsramAllocator& allocator);

			// 创建颜色缓冲区，如果提供了地址则不会分配内存。虚拟地址允许重命名缓冲器（对于跨越帧重用ESRAM特别有用）。？？
			void CreateArray(const wstring& name, uint32_t _width, uint32_t _height, uint32_t arrayCount,
				DXGI_FORMAT& format, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = RenderCore::Resource::GpuVirtualAddressUnknown);

			// 获取CPU可访问的句柄
			const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const;
			// 获取CPU可访问的句柄
			const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV() const;
			// 获取CPU可访问的句柄
			const D3D12_CPU_DESCRIPTOR_HANDLE& GetUAV() const;
			void SetClearColor(Color clearColor);
			void SetMsaaMode(uint32_t numColorSample, uint32_t numCoverageSample);
			Color GetClearColor() const;


		};
	}
}

#endif // !__COLORBUFFER_H__
