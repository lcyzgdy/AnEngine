#pragma once
#ifndef __GPURESOURCE_H__
#define __GPURESOURCE_H__

#include <wrl.h>
#include "DX.h"
#include "Fence.h"
#include <functional>

namespace AnEngine::RenderCore::Resource
{
	static const D3D12_GPU_VIRTUAL_ADDRESS S_GpuVirtualAddressNull = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(0);
	static const D3D12_GPU_VIRTUAL_ADDRESS S_GpuVirtualAddressUnknown = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(-1);

	class GpuResource
	{
		//Fence* m_fence;
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource_cp;
		//Microsoft::WRL::ComPtr<ID3D12Heap> m_heap_cp;
		D3D12_RESOURCE_STATES m_state;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddress;

		// 当使用VirtualAlloc()释放内存时从这里记录。
		void* m_p_userAllocatedMemory;

		virtual void AssociateWithResource(ID3D12Device* device, const std::wstring& name, ID3D12Resource* resource,
			D3D12_RESOURCE_STATES currentState) {};

	public:
		GpuResource(); // For inheritance
		virtual ~GpuResource();

		ID3D12Resource* operator->() { return m_resource_cp.Get(); }
		const ID3D12Resource* operator->() const { return m_resource_cp.Get(); }

		ID3D12Resource* GetResource() { return m_resource_cp.Get(); }
		const ID3D12Resource* GetResource() const { return m_resource_cp.Get(); }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_gpuVirtualAddress; }
	};
}
#endif // !__GPURESOURCE_H__
