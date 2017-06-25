#include "DescriptorHeap.h"
#include "RenderCore.h"
using namespace RenderCore::Resource;

RenderCore::Heap::DescriptorHandle::DescriptorHandle()
{
	m_cpuHandle.ptr = GpuVirtualAddressUnknown;
	m_gpuHandle.ptr = GpuVirtualAddressUnknown;
}

RenderCore::Heap::DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle):
	m_cpuHandle(cpuHandle)
{
	m_gpuHandle.ptr = GpuVirtualAddressUnknown;
}

RenderCore::Heap::DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle):
	m_cpuHandle(cpuHandle), m_gpuHandle(gpuHandle)
{
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderCore::Heap::DescriptorHandle::GetCpuHandle()
{
	return m_cpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderCore::Heap::DescriptorHandle::GetGpuHandle()
{
	return m_gpuHandle;
}

bool RenderCore::Heap::DescriptorHandle::IsNull()
{
	return m_cpuHandle.ptr == GpuVirtualAddressUnknown;
}

bool RenderCore::Heap::DescriptorHandle::IsShaderVisible() const
{
	return m_gpuHandle.ptr != GpuVirtualAddressUnknown;
}

void RenderCore::Heap::DescriptorHandle::operator+=(int offsetScaledByDescriptorSize)
{
	if (m_cpuHandle.ptr != GpuVirtualAddressUnknown)
	{
		m_cpuHandle.ptr += offsetScaledByDescriptorSize;
	}
	if (m_gpuHandle.ptr != GpuVirtualAddressUnknown)
	{
		m_gpuHandle.ptr += offsetScaledByDescriptorSize;
	}
}

RenderCore::Heap::DescriptorHandle RenderCore::Heap::DescriptorHandle::operator+(int offsetScaledByDescriptorSize)
{
	RenderCore::Heap::DescriptorHandle ret = *this;
	ret += offsetScaledByDescriptorSize;
	return ret;
}

