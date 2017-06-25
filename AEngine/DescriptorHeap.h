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
			const uint32_t NumDescriptorPerHeap = 256;
			mutex m_allocatorMutex;
			vector<ComPtr<ID3D12DescriptorHeap>> m_cp_descriptorHeapPool;

		public:
			DescriptorAllocator() = default;
			~DescriptorAllocator() = default;

			D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32_t count);
			void DestoryAll();
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
