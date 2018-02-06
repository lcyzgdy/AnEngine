#pragma once
#ifndef __GPUBUFFER_H__
#define __GPUBUFFER_H__

#include"GpuResource.h"

namespace AnEngine::RenderCore::Resource
{
	class GpuBuffer : public GpuResource
	{
	protected:
		D3D12_RESOURCE_DESC DescribeBuffer(void);
		virtual void CreateDerivedViews(void) = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE m_uav;
		D3D12_CPU_DESCRIPTOR_HANDLE m_srv;

		size_t m_bufferSize;
		uint32_t m_elementCount;
		uint32_t m_elementSize;
		D3D12_RESOURCE_FLAGS m_resourceFlags;
	public:
		GpuBuffer();
		GpuBuffer(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, const void* initialData = nullptr);
		~GpuBuffer();

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetUav(void) const;
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetSrv(void) const;

		D3D12_GPU_VIRTUAL_ADDRESS RootConstantBufferView(void) const;

		D3D12_CPU_DESCRIPTOR_HANDLE CreateConstantBufferView(uint32_t offset, uint32_t size) const;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t offset, uint32_t size, uint32_t stride) const;
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t baseVertexIndex = 0) const;

		D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t offset, uint32_t Size, bool b32Bit = false) const;
		D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t startIndex = 0) const;

		size_t GetBufferSize() const { return m_bufferSize; }
		uint32_t GetElementCount() const { return m_elementCount; }
		uint32_t GetElementSize() const { return m_elementSize; }
	};

	class VertexBuffer : GpuBuffer
	{
	};
}
#endif // !__GPUBUFFER_H__