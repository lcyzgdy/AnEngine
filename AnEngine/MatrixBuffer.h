#pragma once
#ifndef __MATRIXBUFFER_H__
#define __MATRIXBUFFER_H__

#include "GpuResource.h"

namespace AnEngine::RenderCore
{
	class MatrixBuffer
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> m_srvUav;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_srvUavUpload;
		//Microsoft::WRL::ComPtr<ID3D12Resource> m_uavOutput;
		//Microsoft::WRL::ComPtr<ID3D12Resource> m_uavUpload;
		uint32_t m_row;
		uint32_t m_column;

	public:
		MatrixBuffer(uint32_t row, uint32_t column, const float* pData);
	};
}

#endif // !__MATRIXBUFFER_H__
