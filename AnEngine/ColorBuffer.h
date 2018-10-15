#pragma once
#ifndef __COLORBUFFER_H__
#define __COLORBUFFER_H__

#include"DX.h"
#include"PixelBuffer.h"
#include"Color.h"
#include"DescriptorHeap.hpp"

namespace AnEngine::RenderCore::Resource
{
	class ColorBuffer : public PixelBuffer
	{
	protected:
		//Color m_clearColor;
		uint32_t m_numMipMaps;	//子纹理层数量
		uint32_t m_fragmentCount;
		uint32_t m_sampleCount;

	public:
		ColorBuffer() = delete;
		virtual ~ColorBuffer() = default;
		// 创建颜色缓冲区，如果提供了地址则不会分配内存。虚拟地址允许重命名缓冲器（对于跨越帧重用ESRAM特别有用）。？？
		ColorBuffer(const std::wstring& name, uint32_t width, uint32_t height, uint32_t numMips,
			DXGI_FORMAT format, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
			D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = S_GpuVirtualAddressUnknown);
		// 创建颜色缓冲区，提供MSAA支持。此时需要先渲染到内存中支持MSAA的一块缓冲区中，然后再提交到交换链中。
		ColorBuffer(const std::wstring& name, uint32_t width, uint32_t height, uint32_t numMips,
			uint32_t msaaSampleCount, DXGI_FORMAT format, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
			D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = S_GpuVirtualAddressUnknown);
		ColorBuffer(const D3D12_RESOURCE_DESC& desc);
		ColorBuffer(D3D12_RESOURCE_DESC&& desc);
		ColorBuffer(const std::wstring& name, ID3D12Resource* baseResource, D3D12_CPU_DESCRIPTOR_HANDLE handle);

		// 获取CPU可访问的句柄
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetSrv() const { return m_srvHandle; }
		// 获取CPU可访问的句柄
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetRtv() const { return m_rtvHandle; }

		static D3D12_RESOURCE_DESC DescribeAsGBuffer(uint32_t width, uint32_t height);
	};


}

#endif // !__COLORBUFFER_H__