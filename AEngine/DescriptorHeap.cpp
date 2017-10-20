#include "DescriptorHeap.h"
#include "RenderCore.h"
using namespace AEngine::RenderCore::Resource;

namespace AEngine::RenderCore::Heap
{
	DescriptorAllocator r_h_heapDescAllocator;

	DescriptorHandle::DescriptorHandle()
	{
		m_cpuHandle.ptr = GpuVirtualAddressUnknown;
		m_gpuHandle.ptr = GpuVirtualAddressUnknown;
	}

	DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) :
		m_cpuHandle(cpuHandle)
	{
		m_gpuHandle.ptr = GpuVirtualAddressUnknown;
	}

	DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) :
		m_cpuHandle(cpuHandle), m_gpuHandle(gpuHandle)
	{
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle::GetCpuHandle()
	{
		return m_cpuHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle::GetGpuHandle()
	{
		return m_gpuHandle;
	}

	bool DescriptorHandle::IsNull()
	{
		return m_cpuHandle.ptr == GpuVirtualAddressUnknown;
	}

	bool DescriptorHandle::IsShaderVisible() const
	{
		return m_gpuHandle.ptr != GpuVirtualAddressUnknown;
	}

	void DescriptorHandle::operator+=(int offsetScaledByDescriptorSize)
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

	DescriptorHandle DescriptorHandle::operator+(int offsetScaledByDescriptorSize)
	{
		DescriptorHandle t = *this;
		t += offsetScaledByDescriptorSize;
		return t;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	ID3D12DescriptorHeap* DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device)
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

	DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) :
		m_currentHeap{ nullptr }
	{

	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate(
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

	void DescriptorAllocator::DestoryAll()
	{
		m_cp_descriptorHeapPool.clear();
	}
}