#include "DynamicConstantBuffer.h"

static uint32_t Align(uint32_t location, uint32_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)
{
	return (location + (align - 1)) & ~(align - 1);
}
namespace RenderCore
{
	DynamicConstantBuffer::DynamicConstantBuffer(uint32_t constantSize, uint32_t maxDrawsPerFrame, uint32_t frameCount) :
		alignedPerDrawConstantBufferSize(Align(constantSize)),	// Constant buffers must be aligned for hardware requirements.
		maxDrawsPerFrame(maxDrawsPerFrame),
		frameCount(frameCount),
		constantBuffer(nullptr)
	{
		perFrameConstantBufferSize = alignedPerDrawConstantBufferSize * maxDrawsPerFrame;
	}

	DynamicConstantBuffer::~DynamicConstantBuffer()
	{
		constantBuffer->Unmap(0, nullptr);
	}

	void DynamicConstantBuffer::Init(ID3D12Device* pDevice)
	{
		const uint32_t bufferSize = perFrameConstantBufferSize * frameCount;

		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBuffer)
		));


		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pMappedConstantBuffer)));
	}

	void* DynamicConstantBuffer::GetMappedMemory(uint32_t drawIndex, uint32_t frameIndex)
	{
		uint32_t constantBufferOffset = (frameIndex * perFrameConstantBufferSize) + (drawIndex * alignedPerDrawConstantBufferSize);

		uint8_t* temp = reinterpret_cast<uint8_t*>(pMappedConstantBuffer);
		temp += constantBufferOffset;

		return temp;
	}

	D3D12_GPU_VIRTUAL_ADDRESS DynamicConstantBuffer::GetGpuVirtualAddress(uint32_t drawIndex, uint32_t frameIndex)
	{
		uint32_t constantBufferOffset = (frameIndex * perFrameConstantBufferSize) + (drawIndex * alignedPerDrawConstantBufferSize);
		return constantBuffer->GetGPUVirtualAddress() + constantBufferOffset;
	}
}
