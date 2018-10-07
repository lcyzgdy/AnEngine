#pragma once
#ifndef __DEPTHBUFFER_H__
#define __DEPTHBUFFER_H__
#include"DX.h"
#include"PixelBuffer.h"
#include"DescriptorHeap.hpp"
using namespace std;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::RenderCore::Resource
{
	class DepthBuffer :public PixelBuffer
	{
	protected:
		float m_clearDepth;

		D3D12_RESOURCE_DESC DescribeAsDepthBuffer();

	public:
		DepthBuffer() = delete;
		DepthBuffer(uint32_t width, uint32_t height, float clearDepth = 0.0f);
		virtual ~DepthBuffer() = default;

		float GetClearDepth() const { return m_clearDepth; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() { return m_dsvHandle; }
	};
}


#endif // !__DEPTHBUFFER_H__
