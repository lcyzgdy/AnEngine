#include "GpuResource.h"
#include "RenderCore.h"
using namespace std;
using namespace Microsoft::WRL;

namespace AnEngine::RenderCore::Resource
{
	GpuResource::GpuResource() :
		m_gpuVirtualAddress(S_GpuVirtualAddressNull), m_p_userAllocatedMemory(nullptr), m_state(D3D12_RESOURCE_STATE_COMMON)
	{
	}

	GpuResource::~GpuResource()
	{
		m_gpuVirtualAddress = S_GpuVirtualAddressNull;
		if (m_p_userAllocatedMemory != nullptr)
		{
			VirtualFree(m_p_userAllocatedMemory, 0, MEM_RELEASE);
			m_p_userAllocatedMemory = nullptr;
		}
	}
}