#pragma once
#ifndef __GPURESOURCE_H__
#define __GPURESOURCE_H__

#include<wrl.h>
#include"DX.h"

using namespace std;
using namespace Microsoft::WRL;

namespace RenderCore
{
	namespace Resource
	{
		static const D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddressNull = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(0);
		class GpuResource
		{
		protected:
			ComPtr<ID3D12Resource> m_resource;
			D3D12_RESOURCE_STATES m_usageState;
			D3D12_RESOURCE_STATES m_transitioningState;
			D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddress;

			// 当使用VirtualAlloc()释放内存时从这里记录。
			void* m_userAllocatedMemory;
		public:
			GpuResource();
			~GpuResource() = default;

			virtual void Release();

			ID3D12Resource* operator->();
			const ID3D12Resource* operator->() const;

			ID3D12Resource* GetResource();
			const ID3D12Resource* GetResource() const;

			D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;
		};
	}
}
#endif // !__GPURESOURCE_H__
