#include "GpuResource.h"
#include "RenderCore.h"
using namespace std;
using namespace Microsoft::WRL;

namespace AnEngine::RenderCore::Resource
{
	GpuResource::GpuResource() :
		m_gpuVirtualAddress(RenderCore::Resource::GpuVirtualAddressNull),
		m_p_userAllocatedMemory(nullptr),
		m_usageState(D3D12_RESOURCE_STATE_COMMON),
		m_transitioningState((D3D12_RESOURCE_STATES)-1)
	{
		//m_fence = new Fence(r_graphicsCard[0]->GetCommandQueue());
	}

	GpuResource::GpuResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState) :
		m_gpuVirtualAddress(Resource::GpuVirtualAddressNull),
		m_p_userAllocatedMemory(nullptr),
		m_resource_cp(resource),
		m_usageState(currentState),
		m_transitioningState(static_cast<D3D12_RESOURCE_STATES>(-1))
	{
		//m_fence = new Fence(r_graphicsCard[0]->GetCommandQueue());
	}

	GpuResource::~GpuResource()
	{
		m_gpuVirtualAddress = RenderCore::Resource::GpuVirtualAddressNull;
		if (m_p_userAllocatedMemory != nullptr)
		{
			VirtualFree(m_p_userAllocatedMemory, 0, MEM_RELEASE);
			m_p_userAllocatedMemory = nullptr;
		}
		//delete m_fence;
	}

	/*void GpuResource::Release()
	{
		//delete m_resource_cp.Get();
		//m_resource_cp = nullptr;
	}*/

	ID3D12Resource* GpuResource::operator->()
	{
		return m_resource_cp.Get();
	}

	const ID3D12Resource* GpuResource::operator->() const
	{
		return m_resource_cp.Get();
	}

	ID3D12Resource* GpuResource::GetResource()
	{
		return m_resource_cp.Get();
	}

	const ID3D12Resource* GpuResource::GetResource() const
	{
		return m_resource_cp.Get();
	}

	D3D12_GPU_VIRTUAL_ADDRESS GpuResource::GetGpuVirtualAddress() const
	{
		return m_gpuVirtualAddress;
	}

	/*Fence* GpuResource::GetFence()
	{
		return m_fence;
	}*/
}