#pragma once
#ifndef __DEPTHBUFFER_H__
#define __DEPTHBUFFER_H__
#include"DX.h"
#include"PixelBuffer.h"
#include"DescriptorHeap.hpp"
using namespace std;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::RenderCore::Resource
{
	class DepthBuffer :public PixelBuffer
	{
	protected:
		float m_clearDepth;
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle[4];
		D3D12_CPU_DESCRIPTOR_HANDLE m_depthSrvHandle;

		/*void CreateDerviedViews(ID3D12Device* device, DXGI_FORMAT& format,
			Heap::DescriptorHeapAllocator* descAllocator);*/
		D3D12_RESOURCE_DESC DescribeAsDepthBuffer();
	public:
		DepthBuffer() = delete;
		DepthBuffer(uint32_t width, uint32_t height, float clearDepth = 0.0f);
		virtual ~DepthBuffer() = default;

		// 创建颜色缓冲区，如果提供了地址则不会分配内存。虚拟地址允许重命名缓冲器（对于跨越帧重用ESRAM特别有用）。？？
		/*void Create(const wstring& name, uint32_t _width, uint32_t _height, DXGI_FORMAT& format,
			RenderCore::Heap::DescriptorHeapAllocator* descAllocator, uint32_t numSamples = 1,
			D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = RenderCore::Resource::S_GpuVirtualAddressUnknown);*/

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetDsv() const { return m_dsvHandle[0]; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetDsvDepthReadOnly() const { return m_dsvHandle[1]; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetDsvStencilReadOnly() const { return m_dsvHandle[2]; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetDsvReadOnly() const { return m_dsvHandle[3]; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetDepthSrv() const { return m_depthSrvHandle; }
		//const D3D12_CPU_DESCRIPTOR_HANDLE& GetStencilSrv() const { return m_stencilSrvHandle; }

		float GetClearDepth() const { return m_clearDepth; }
		//uint8_t GetClearStencil() const { return m_clearStencil; }
	};
}


#endif // !__DEPTHBUFFER_H__
