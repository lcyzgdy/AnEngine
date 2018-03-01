#include "MatrixBuffer.h"
#include "RenderCore.h"
#include "CommandContext.h"
#include "DescriptorHeap.hpp"
using namespace std;
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::RenderCore
{
	MatrixBuffer::MatrixBuffer(uint32_t row, uint32_t column, const float* pData) : m_row(row), m_column(column)
	{
		var device = r_graphicsCard[0]->GetDevice();
		var[pList, pAllocator] = GraphicsContext::GetOne();
		var iList = pList->GetCommandList();
		var iAllocator = pAllocator->GetAllocator();

		uint64_t dataSize = row * column * sizeof(float);
		D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		D3D12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);

		ThrowIfFailed(device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE,
			&bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_srvUav)));
		ThrowIfFailed(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_srvUavUpload)));
		/*ThrowIfFailed(device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE,
			&bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_uavOutput)));
		ThrowIfFailed(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_uavUpload)));*/

		D3D12_SUBRESOURCE_DATA matrix = {};
		matrix.pData = pData;
		matrix.RowPitch = dataSize;
		matrix.SlicePitch = dataSize;

		UpdateSubresources<1>(iList, m_srvUav.Get(), m_srvUavUpload.Get(), 0, 0, 1, &matrix);
		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_srvUav.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = row * column;
		srvDesc.Buffer.StructureByteStride = sizeof(float);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	}
}