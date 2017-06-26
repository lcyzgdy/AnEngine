#pragma once
#ifndef __DESCRIPTORHEAP_H__
#define __DESCRIPTORHEAP_H__

#include<mutex>
#include<vector>
#include<string>
#include<queue>
#include"DX.h"

namespace RenderCore
{
	namespace Heap
	{
		class DescriptorAllocator
		{
		protected:
			const uint32_t m_NumDescriptorPerHeap = 256;
			mutex m_allocatorMutex;
			vector<ComPtr<ID3D12DescriptorHeap>> m_cp_descriptorHeapPool;

			//D3D12_DESCRIPTOR_HEAP_TYPE m_type[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
			ID3D12DescriptorHeap* m_currentHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
			D3D12_CPU_DESCRIPTOR_HANDLE m_currentHandle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
			uint32_t m_descriptorSize[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
			uint32_t m_remainingFreeHandles[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

			ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device);

		public:
			DescriptorAllocator() = default;
			DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type);
			~DescriptorAllocator() = default;

			D3D12_CPU_DESCRIPTOR_HANDLE Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Device* device,
				uint32_t count = 1);
			void DestoryAll();

			//D3D12_CPU_DESCRIPTOR_HANDLE operator[](D3D12_DESCRIPTOR_HEAP_TYPE type);
		};



		class DescriptorHandle
		{
			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
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
	}
}
#endif // !__DESCRIPTORHEAP_H__
