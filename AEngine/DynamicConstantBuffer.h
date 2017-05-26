#pragma once
#ifndef __DYNAMICCONSTANTBUFFER_H__
#define __DYNAMICCONSTANTBUFFER_H__

#include"onwind.h"
#include"DX.h"

class DynamicConstantBuffer
{
public:
	DynamicConstantBuffer(UINT constantSize, UINT maxDrawsPerFrame, UINT frameCount);
	~DynamicConstantBuffer();

	void Init(ID3D12Device* pDevice);
	void* GetMappedMemory(UINT drawIndex, UINT frameIndex);
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(UINT drawIndex, UINT frameIndex);

private:
	ComPtr<ID3D12Resource> constantBuffer;
	void* pMappedConstantBuffer;
	UINT  alignedPerDrawConstantBufferSize;
	UINT  perFrameConstantBufferSize;

	UINT frameCount;
	UINT maxDrawsPerFrame;
};

#endif // !__DYNAMICCONSTANTBUFFER_H__
