#pragma once
#ifndef __DESCRIPTORHEAP_H__
#define __DESCRIPTORHEAP_H__

#include <mutex>
#include <vector>
#include <string>
#include <queue>
#include <tuple>
#include "onwind.h"
#include "DX.h"
#include "Singleton.h"

/*
namespace AnEngine::RenderCore
{
	void InitializeRender(HWND hwnd, int graphicCardCount, bool isStable);
}
*/

namespace AnEngine::RenderCore
{
	class DescriptorHeapAllocator : public Utility::Singleton<DescriptorHeapAllocator>
	{
		// static DescriptorHeapAllocator* m_uniqueObj;
		friend Utility::Singleton<DescriptorHeapAllocator>;
	protected:
		const uint32_t m_NumDescriptorPerHeap = 256;
		std::mutex m_mutex;
		std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> m_cp_descriptorHeapPool;

		//D3D12_DESCRIPTOR_HEAP_TYPE m_type[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_currentHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentHandle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		D3D12_GPU_DESCRIPTOR_HANDLE m_currentGpuHandle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		uint32_t m_descriptorSize[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		uint32_t m_remainingFreeHandles[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		void RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_FLAGS flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		void DestoryAll();

		DescriptorHeapAllocator() = default;
		~DescriptorHeapAllocator();

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t count = 1);
		std::tuple<ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> Allocate2(D3D12_DESCRIPTOR_HEAP_TYPE type,
			uint32_t count = 1, D3D12_DESCRIPTOR_HEAP_FLAGS flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

		// static DescriptorHeapAllocator* GetInstance();

		//D3D12_CPU_DESCRIPTOR_HANDLE operator[](D3D12_DESCRIPTOR_HEAP_TYPE type);
	};



	class DescriptorHandle
	{
		//template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType> friend class DescriptorHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
		void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle);
		void SetGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);

	public:
		DescriptorHandle();
		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
		~DescriptorHandle() = default;

		D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle();


		bool IsNull();
		bool IsShaderVisible() const;

		void operator+=(int offsetScaledByDescriptorSize);
		DescriptorHandle operator+(int offsetScaledByDescriptorSize);
	};

	/*
	template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
	class DescriptorHeap : NonCopyable
	{
		friend void ::AnEngine::RenderCore::InitializeRender(HWND hwnd, int graphicCardCount, bool isStable);

		inline static uint32_t m_numDescripotrPerHeap = 256;
		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		uint32_t m_referenceCount;

		DescriptorHandle m_handle;
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;

		void Create(ID3D12Device* device)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			desc.NumDescriptors = m_numDescripotrPerHeap;
			desc.Type = HeapType;
			ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descriptorHeap)));
			m_cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_handle.SetCpuHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
			m_handle.SetGpuHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
		}

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
			ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descriptorHeap)));
			m_cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_handle.SetCpuHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
			m_handle.SetGpuHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
		}

		explicit DescriptorHeap()
		{
			if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			{
				m_referenceCount++;
				if (m_referenceCount > 1) return;
			}
		}

		~DescriptorHeap()
		{
			if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			{
				m_referenceCount--;
				if (m_referenceCount > 0) return;
			}
			//m_descriptorHeap.Release();
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
		}

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

		void OffsetHandle(uint32_t size)
		{

		}
	};*/
}
#endif // !__DESCRIPTORHEAP_H__
