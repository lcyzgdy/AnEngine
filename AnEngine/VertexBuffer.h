#pragma once
#ifndef __VERTEXBUFFER_H__
#define __VERTEXBUFFER_H__

#include"GpuBuffer.h"
#include<vector>

namespace AnEngine::RenderCore::Resource
{
	class VertexBuffer
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBufferUpload;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBufferUpload;

		std::byte* m_data;
		size_t m_dataSize;

		D3D12_VERTEX_BUFFER_VIEW m_vbv;
		D3D12_INDEX_BUFFER_VIEW m_ibv;
	public:
		VertexBuffer(D3D12_SUBRESOURCE_DATA& subData, size_t vertexDataSize, uint32_t standardVertexStride, D3D12_SUBRESOURCE_DATA& indexData,
			size_t indexDataSize, DXGI_FORMAT standardIndexFormat);

		D3D12_VERTEX_BUFFER_VIEW GetVbv();
		D3D12_INDEX_BUFFER_VIEW GetIbv();
	};
}

#endif // !__VERTEXBUFFER_H__
