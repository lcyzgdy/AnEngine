#pragma once
#ifndef __GPURESOURCE_H__
#define __GPURESOURCE_H__

#include<wrl.h>
#include"DX.h"
#include"Fence.hpp"
#include<functional>

namespace AnEngine::RenderCore::Resource
{
	static const D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddressNull = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(0);
	static const D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddressUnknown = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(-1);

	class GpuResource
	{
		//Fence* m_fence;
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource_cp;
		Microsoft::WRL::ComPtr<ID3D12Heap> m_heap_cp;
		D3D12_RESOURCE_STATES m_usageState;
		D3D12_RESOURCE_STATES m_transitioningState;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddress;

		// 当使用VirtualAlloc()释放内存时从这里记录。
		void* m_p_userAllocatedMemory;
	public:
		GpuResource(); // For inheritance
		GpuResource(ID3D12Resource* p_resource, D3D12_RESOURCE_STATES currentState);
		virtual ~GpuResource();

		//virtual void Release();

		ID3D12Resource* operator->();
		const ID3D12Resource* operator->() const;

		ID3D12Resource* GetResource();
		const ID3D12Resource* GetResource() const;

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;

		//Fence* GetFence();

		void Create();
	};
}
#endif // !__GPURESOURCE_H__
