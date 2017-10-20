#include "ColorBuffer.h"
//#include "DescriptorHeap.h"
#include "RenderCoreConstants.h"
using namespace AEngine::RenderCore;
using namespace AEngine::RenderCore::Resource;

namespace AEngine::RenderCore::Resource
{
	ColorBuffer::ColorBuffer() :
		PixelBuffer(), m_clearColor(Color(0.0f, 0.0f, 0.0f)),
		m_numMipMaps(0), m_fragmentCount(1), m_sampleCount(1)
	{
	}

	ColorBuffer::ColorBuffer(Color clearColor) :
		m_clearColor(clearColor), m_numMipMaps(0), m_fragmentCount(1), m_sampleCount(1)
	{
		m_rtvHandle.ptr = GpuVirtualAddressUnknown;
		m_srvHandle.ptr = GpuVirtualAddressUnknown;
		memset(m_uavHandle, 0xff, sizeof(m_uavHandle));
	}

	void ColorBuffer::CreateFromSwapChain(const wstring& name, ID3D12Resource* baseResource,
		ID3D12Device* device, RenderCore::Heap::DescriptorAllocator* heapDescAllocator)
	{
		// 和资源进行关联，状态是present（呈现）。
		AssociateWithResource(device, name, baseResource, D3D12_RESOURCE_STATE_PRESENT);
		m_rtvHandle = heapDescAllocator->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, device);
		device->CreateRenderTargetView(m_cp_resource.Get(), nullptr, m_rtvHandle);
	}

	D3D12_RESOURCE_FLAGS ColorBuffer::CombineResourceFlags() const
	{
		return D3D12_RESOURCE_FLAGS();
	}

	uint32_t ColorBuffer::ComputeNumMips(uint32_t width, uint32_t height)
	{
		return uint32_t();
	}

	void ColorBuffer::CreateDerivedViews(ID3D12Device * device, DXGI_FORMAT & format, uint32_t arraySize, uint32_t numMips)
	{
	}

	void ColorBuffer::Create(const wstring & name, uint32_t _width, uint32_t _height, uint32_t numMips, DXGI_FORMAT & format, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr)
	{
	}

	void ColorBuffer::CreateArray(const wstring & name, uint32_t _width, uint32_t _height, uint32_t arrayCount, DXGI_FORMAT & format, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr)
	{
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE & ColorBuffer::GetSRV() const
	{
		return m_srvHandle;
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE & ColorBuffer::GetRTV() const
	{
		return m_rtvHandle;
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE & ColorBuffer::GetUAV() const
	{
		return m_uavHandle[0];
	}

	void ColorBuffer::SetClearColor(Color clearColor)
	{
	}

	void ColorBuffer::SetMsaaMode(uint32_t numColorSample, uint32_t numCoverageSample)
	{
	}

	Color ColorBuffer::GetClearColor() const
	{
		return Color();
	}
}