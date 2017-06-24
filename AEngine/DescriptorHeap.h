#pragma once
#ifndef __DESCRIPTORHEAP_H__
#define __DESCRIPTORHEAP_H__

#include<mutex>
#include<vector>
#include<string>
#include<queue>
#include"DX.h"

class DescriptorAllocator
{
protected:
	static const uint32_t NumDescriptorPerHeap = 256;
	static mutex m_allocatorMutex;
	static vector<ComPtr<ID3D12DescriptorHeap>> m_descriptorHeapPool;

public:
	DescriptorAllocator() = default;
	~DescriptorAllocator() = default;

	D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32_t count);
	void DestoryAll();
};

#endif // !__DESCRIPTORHEAP_H__
