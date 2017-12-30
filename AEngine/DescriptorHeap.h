#pragma once
#ifndef __DESCRIPTORHEAP_H__
#define __DESCRIPTORHEAP_H__

#include<mutex>
#include<vector>
#include<string>
#include<queue>
#include"DX.h"

namespace AEngine::RenderCore::Heap
{
	class DescriptorHeapAllocator
	{
		// RTV, SRV, UAV, 
	protected:
		const uint32_t m_NumDescriptorPerHeap = 256;
		mutex m_mutex;
		vector<ComPtr<ID3D12DescriptorHeap>> m_cp_descriptorHeapPool;

		//D3D12_DESCRIPTOR_HEAP_TYPE m_type[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		ID3D12DescriptorHeap* m_currentHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentHandle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		uint32_t m_descriptorSize[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		uint32_t m_remainingFreeHandles[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device);

	public:
		DescriptorHeapAllocator() = default;
		DescriptorHeapAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type);
		~DescriptorHeapAllocator() = default;

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device,
			uint32_t count = 1);
		void DestoryAll();

		static DescriptorHeapAllocator* GetInstance();

		//D3D12_CPU_DESCRIPTOR_HANDLE operator[](D3D12_DESCRIPTOR_HEAP_TYPE type);
	};



	class DescriptorHandle
	{
		template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType> friend class DescriptorHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
		void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle);
		void SetGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);

	public:
		DescriptorHandle();
		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
		~DescriptorHandle() = default;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle();


		bool IsNull();
		bool IsShaderVisible() const;

		void operator+=(int offsetScaledByDescriptorSize);
		DescriptorHandle operator+(int offsetScaledByDescriptorSize);
	};


	template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
	class DescriptorHeap : NonCopyable
	{
		inline static uint32_t m_numDescripotrPerHeap = 256;
		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		uint32_t m_referenceCount;

		DescriptorHandle m_handle;

	public:
		explicit DescriptorHeap(ID3D12Device* device)
		{
			if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			{
				m_referenceCount++;
				if (m_referenceCount > 1) return;
			}
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			desc.NumDescriptors = m_numDescripotrPerHeap;
			desc.Type = HeapType;
			ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap)));

			m_handle.SetCpuHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
			m_handle.SetGpuHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
		}

		~DescriptorHeap()
		{
			if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			{
				m_referenceCount--;
				if (m_referenceCount > 0) return;
			}
			m_descriptorHeap->Release();
		}

		void* operator new(size_t size) noexcept(false)
		{
			if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			{
				static DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> m_s_rtvDescriptorHeap;
				return &m_s_rtvDescriptorHeap;
			}
			return ::operator new(size);
		}

		/*static void* operator new(size_t size, void* cachePointer)
		{
			return cachePointer;
		}*/

		void operator delete(void* ptr)
		{
			if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV
				&& static_cast<DescriptorHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>*>(ptr)->m_referenceCount > 0)
				return;
			::operator delete(ptr);
		}

		ID3D12DescriptorHeap* GetDescriptorHeap()
		{
			return m_descriptorHeap.Get();
		}

		void SetNumDescriportPerHeap(uint32_t num)
		{
			m_numDescripotrPerHeap = num;
		}

		DescriptorHandle GetHandle()
		{
			return m_handle;
		}
	};
}
#endif // !__DESCRIPTORHEAP_H__
