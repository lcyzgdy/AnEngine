#pragma once
#ifndef __DYNAMICCONSTANTBUFFER_H__
#define __DYNAMICCONSTANTBUFFER_H__

#include"onwind.h"
#include"DX.h"

namespace RenderCore
{
	class DynamicConstantBuffer
	{
	public:
		DynamicConstantBuffer(uint32_t constantSize, uint32_t maxDrawsPerFrame, uint32_t frameCount);
		~DynamicConstantBuffer();

		void Init(ID3D12Device* pDevice);
		void* GetMappedMemory(uint32_t drawIndex, uint32_t frameIndex);
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32_t drawIndex, uint32_t frameIndex);

	private:
		ComPtr<ID3D12Resource> constantBuffer;
		void* pMappedConstantBuffer;
		uint32_t  alignedPerDrawConstantBufferSize;
		uint32_t  perFrameConstantBufferSize;

		uint32_t frameCount;
		uint32_t maxDrawsPerFrame;
	};
}
#endif // !__DYNAMICCONSTANTBUFFER_H__
