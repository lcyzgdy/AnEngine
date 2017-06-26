#include "DescriptorHeap.h"
#include "RenderCore.h"
using namespace RenderCore::Resource;

RenderCore::Heap::DescriptorAllocator RenderCore::Heap::r_h_heapDescAllocator;

RenderCore::Heap::DescriptorHandle::DescriptorHandle()
{
	m_cpuHandle.ptr = GpuVirtualAddressUnknown;
	m_gpuHandle.ptr = GpuVirtualAddressUnknown;
}

RenderCore::Heap::DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) :
	m_cpuHandle(cpuHandle)
{
	m_gpuHandle.ptr = GpuVirtualAddressUnknown;
}

RenderCore::Heap::DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) :
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
	RenderCore::Heap::DescriptorHandle t = *this;
	t += offsetScaledByDescriptorSize;
	return t;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

ID3D12DescriptorHeap* RenderCore::Heap::DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device)
{
	lock_guard<mutex> lockGuard(m_allocatorMutex);

	D3D12_DESCRIPTOR_HEAP_DESC desc;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 1;
	desc.NumDescriptors = m_NumDescriptorPerHeap;
	desc.Type = type;

	ComPtr<ID3D12DescriptorHeap> cp_heap;
	ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(cp_heap.GetAddressOf())));
	m_cp_descriptorHeapPool.emplace_back(cp_heap);
	return cp_heap.Get();
}

RenderCore::Heap::DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type):
	m_currentHeap{ nullptr }
{

}

D3D12_CPU_DESCRIPTOR_HANDLE RenderCore::Heap::DescriptorAllocator::Allocate(
	D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device, uint32_t count)
{
	if (m_currentHeap == nullptr || m_remainingFreeHandles[type] < count)
	{
		m_currentHeap[type] = RequestNewHeap(type, device);
		m_currentHandle[type] = m_currentHeap[type]->GetCPUDescriptorHandleForHeapStart();
		m_remainingFreeHandles[type] = m_NumDescriptorPerHeap;

		(m_descriptorSize[type] == 0) ? m_descriptorSize[type] = device->GetDescriptorHandleIncrementSize(type) : (0);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_currentHandle[type];
	m_currentHandle[type].ptr += count * m_descriptorSize[type];
	m_remainingFreeHandles[type] -= count;
	return cpuHandle;
}

void RenderCore::Heap::DescriptorAllocator::DestoryAll()
{
	m_cp_descriptorHeapPool.clear();
}
