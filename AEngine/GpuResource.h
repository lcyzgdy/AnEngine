#pragma once
#ifndef __GPURESOURCE_H__
#define __GPURESOURCE_H__

#include<wrl.h>
#include"DX.h"

using namespace std;
using namespace Microsoft::WRL;

namespace AEngine::RenderCore::Resource
{
	static const D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddressNull = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(0);
	static const D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddressUnknown = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(-1);

	class GpuResource
	{
	protected:
		ComPtr<ID3D12Resource> m_cp_resource;
		D3D12_RESOURCE_STATES m_usageState;
		D3D12_RESOURCE_STATES m_transitioningState;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddress;

		// 当使用VirtualAlloc()释放内存时从这里记录。
		void* m_p_userAllocatedMemory;
	public:
		GpuResource();
		GpuResource(ID3D12Resource* p_resource, D3D12_RESOURCE_STATES currentState);
		~GpuResource() = default;

		virtual void Release();

		ID3D12Resource* operator->();
		const ID3D12Resource* operator->() const;

		ID3D12Resource* GetResource();
		const ID3D12Resource* GetResource() const;

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;
	};
}
#endif // !__GPURESOURCE_H__
