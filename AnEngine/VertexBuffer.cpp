#include "VertexBuffer.h"
#include "RenderCore.h"
#include "CommandContext.h"
using namespace AnEngine::RenderCore;

namespace AnEngine::RenderCore::Resource
{
	VertexBuffer::VertexBuffer(D3D12_SUBRESOURCE_DATA& subData, size_t vertexDataSize, uint32_t standardVertexStride)
	{
		var device = r_graphicsCard[0]->GetDevice();
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();
		var iAllocator = commandAllocator->GetAllocator();
		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, nullptr));

		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)));
		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&m_vertexBufferUpload)));

		UpdateSubresources<1>(iList, m_vertexBuffer.Get(), m_vertexBufferUpload.Get(), 0, 0, 1, &subData);
		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_vbv.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vbv.SizeInBytes = vertexDataSize;
		m_vbv.StrideInBytes = standardVertexStride;

		iList->Close();
		ID3D12CommandList* ppCommandLists[] = { iList };
		r_graphicsCard[0]->ExecuteSync(_countof(ppCommandLists), ppCommandLists);

		GraphicsContext::Push(commandList, commandAllocator);
	}

	VertexBuffer::VertexBuffer(D3D12_SUBRESOURCE_DATA& subData, size_t vertexDataSize, uint32_t standardVertexStride,
		D3D12_SUBRESOURCE_DATA& indexData, size_t indexDataSize,
		DXGI_FORMAT standardIndexFormat)
	{
		var device = r_graphicsCard[0]->GetDevice();
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();

		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)));
		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&m_vertexBufferUpload)));

		UpdateSubresources<1>(iList, m_vertexBuffer.Get(), m_vertexBufferUpload.Get(), 0, 0, 1, &subData);
		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_vbv.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vbv.SizeInBytes = vertexDataSize;
		m_vbv.StrideInBytes = standardVertexStride;

		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indexDataSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
			IID_PPV_ARGS(&m_indexBuffer)));
		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indexDataSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&m_indexBufferUpload)));

		UpdateSubresources<1>(iList, m_indexBuffer.Get(), m_indexBufferUpload.Get(), 0, 0, 1, &indexData);
		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		m_ibv.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_ibv.SizeInBytes = indexDataSize;
		m_ibv.Format = standardIndexFormat;

		iList->Close();
		ID3D12CommandList* ppCommandLists[] = { iList };
		r_graphicsCard[0]->ExecuteSync(_countof(ppCommandLists), ppCommandLists);

		GraphicsContext::Push(commandList, commandAllocator);
	}
}