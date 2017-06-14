#include "GpuResource.h"

namespace RenderCore
{
	namespace Resource
	{
		GpuResource::GpuResource() :
			m_gpuVirtualAddress(RenderCore::Resource::GpuVirtualAddressNull),
			m_userAllocatedMemory(nullptr),
			m_usageState(D3D12_RESOURCE_STATE_COMMON),
			m_transitioningState((D3D12_RESOURCE_STATES)-1)
		{
		}

		void GpuResource::Release()
		{

			m_resource = nullptr;
			m_gpuVirtualAddress = RenderCore::Resource::GpuVirtualAddressNull;
			if (m_userAllocatedMemory != nullptr)
			{
				VirtualFree(m_userAllocatedMemory, 0, MEM_RELEASE);
				m_userAllocatedMemory = nullptr;
			}
		}

		ID3D12Resource * GpuResource::operator->()
		{
			return m_resource.Get();
		}

		const ID3D12Resource * GpuResource::operator->() const
		{
			return m_resource.Get();
		}

		ID3D12Resource * GpuResource::GetResource()
		{
			return m_resource.Get();
		}

		const ID3D12Resource * GpuResource::GetResource() const
		{
			return m_resource.Get();
		}

		D3D12_GPU_VIRTUAL_ADDRESS GpuResource::GetGpuVirtualAddress() const
		{
			return m_gpuVirtualAddress;
		}
	}
}