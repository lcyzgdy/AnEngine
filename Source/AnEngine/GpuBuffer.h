#pragma once
#ifndef __GPUBUFFER_H__
#define __GPUBUFFER_H__

#include"GpuResource.h"

namespace AnEngine::RenderCore::Resource
{
	class GpuBuffer : public GpuResource
	{
	protected:
		D3D12_RESOURCE_DESC DescribeBuffer();
		virtual void CreateDerivedViews() = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE m_uav;
		D3D12_CPU_DESCRIPTOR_HANDLE m_srv;

		size_t m_bufferSize;
		uint32_t m_elementCount;
		uint32_t m_elementSize;
		D3D12_RESOURCE_FLAGS m_resourceFlags;
	public:
		GpuBuffer(const std::wstring& name, uint32_t numElements, uint32_t elementSize,
			const void* initialData = nullptr);
		~GpuBuffer();

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetUav() const;
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetSrv() const;

		D3D12_GPU_VIRTUAL_ADDRESS RootConstantBufferView() const;

		D3D12_CPU_DESCRIPTOR_HANDLE CreateConstantBufferView(uint32_t offset, uint32_t size) const;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t offset, uint32_t size, uint32_t stride) const;
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t baseVertexIndex = 0) const;

		D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t offset, uint32_t size, bool b32Bit = false) const;
		D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t startIndex = 0) const;

		size_t GetBufferSize() const { return m_bufferSize; }
		uint32_t GetElementCount() const { return m_elementCount; }
		uint32_t GetElementSize() const { return m_elementSize; }
	};

	class ByteAddressBuffer : public GpuBuffer
	{
	protected:
		virtual void CreateDerivedViews() override;
	public:
		ByteAddressBuffer(const std::wstring& name, uint32_t numElements, uint32_t elementSize,
			const void* initialData = nullptr);
	};

	class GpuUploadBuffer
	{
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() { return m_resource; }

	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;

		GpuUploadBuffer() {}
		~GpuUploadBuffer();

		void Allocate(ID3D12Device* device, uint32_t bufferSize, wchar_t* resourceName = nullptr);

		uint8_t* MapCpuWriteOnly();
	};
}
#endif // !__GPUBUFFER_H__