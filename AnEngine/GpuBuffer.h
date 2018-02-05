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
		~GpuBuffer();

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetUav(void) const;
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetSrv(void) const;

		D3D12_GPU_VIRTUAL_ADDRESS RootConstantBufferView(void) const;

		D3D12_CPU_DESCRIPTOR_HANDLE CreateConstantBufferView(uint32_t Offset, uint32_t Size) const;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t Offset, uint32_t Size, uint32_t Stride) const;
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t BaseVertexIndex = 0) const
		{
			size_t Offset = BaseVertexIndex * m_elementSize;
			return VertexBufferView(Offset, (uint32_t)(m_bufferSize - Offset), m_elementSize);
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t Offset, uint32_t Size, bool b32Bit = false) const;
		D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t StartIndex = 0) const
		{
			size_t Offset = StartIndex * m_elementSize;
			return IndexBufferView(Offset, (uint32_t)(m_bufferSize - Offset), m_elementSize == 4);
		}

		size_t GetBufferSize() const { return m_bufferSize; }
		uint32_t GetElementCount() const { return m_elementCount; }
		uint32_t GetElementSize() const { return m_elementSize; }
	};

	class VertexBuffer : GpuBuffer
	{
	};
}
#endif // !__GPUBUFFER_H__