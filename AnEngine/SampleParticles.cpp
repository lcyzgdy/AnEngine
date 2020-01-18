#include "SampleParticles.h"
#include "RenderCore.h"
#include "CommandContext.h"
#include "DescriptorHeap.hpp"

using namespace std;
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Heap;
using namespace DirectX;

namespace AnEngine::RenderCore::Resource
{
	const float SampleParticles::ParticleSpread = 400.0f;

	SampleParticles::SampleParticles()
	{
		var device = r_graphicsCard[0]->GetDevice();
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();
		var iAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iAllocator->Reset(), R_GetGpuError);
		ThrowIfFailed(iList->Reset(iAllocator, nullptr), R_GetGpuError);

		vector<ParticleVertex> vertices;
		Randomize();
		vertices.resize(ParticleCount);
		for (int i = 0; i < ParticleCount; i++)
		{
			vertices[i].color = XMFLOAT4(Random(0.0f, 1.0f), Random(0.0f, 1.0f), Random(0.0f, 1.0f), 1.0f);
		}
		const uint32_t bufferSize = ParticleCount * sizeof(ParticleVertex);

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<uint8_t*>(&vertices[0]);
		vertexData.RowPitch = bufferSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		m_vertexBuffer = new VertexBuffer(vertexData, bufferSize, sizeof(ParticleVertex));

		vector<Particle> data;
		data.resize(ParticleCount);
		const UINT dataSize = ParticleCount * sizeof(Particle);
		// 缓冲区初始化数据

		float centerSpread = ParticleSpread * 0.50f;
		InitializeParticles(&data[0], XMFLOAT3(centerSpread, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, -20.0f, 1.0f / 100000000.0f), ParticleSpread, ParticleCount);

		D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		D3D12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);

		ThrowIfFailed(device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_particleBuffer0)), R_GetGpuError);
		ThrowIfFailed(device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_particleBuffer1)), R_GetGpuError);
		ThrowIfFailed(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_particleBuffer0Upload)), R_GetGpuError);
		ThrowIfFailed(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_particleBuffer1Upload)), R_GetGpuError);

		D3D12_SUBRESOURCE_DATA particleData = {};
		particleData.pData = reinterpret_cast<uint8_t*>(&data[0]);
		particleData.RowPitch = dataSize;
		particleData.SlicePitch = particleData.RowPitch;

		UpdateSubresources<1>(iList, m_particleBuffer0.Get(), m_particleBuffer0Upload.Get(), 0, 0, 1, &particleData);
		UpdateSubresources<1>(iList, m_particleBuffer1.Get(), m_particleBuffer1Upload.Get(), 0, 0, 1, &particleData);
		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_particleBuffer0.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		iList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_particleBuffer1.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

		/*var[srvUavHeap, srvUavHandle, srvUavGpuHandle] = DescriptorHeapAllocator::GetInstance()->Allocate2(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			DescriptorCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		m_srvUavHeap = srvUavHeap;
		m_srvUavHandle = srvUavHandle;
		m_srvUavGpuHandle = srvUavGpuHandle;*/
		D3D12_DESCRIPTOR_HEAP_DESC srvUavHeapDesc = {};
		srvUavHeapDesc.NumDescriptors = DescriptorCount;
		srvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(device->CreateDescriptorHeap(&srvUavHeapDesc, IID_PPV_ARGS(&m_srvUavHeap)), R_GetGpuError);
		m_srvUavHandle = m_srvUavHeap->GetCPUDescriptorHandleForHeapStart();
		m_srvUavGpuHandle = m_srvUavHeap->GetGPUDescriptorHandleForHeapStart();

		//srvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		uint32_t srvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = ParticleCount;
		srvDesc.Buffer.StructureByteStride = sizeof(Particle);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		//CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle0(m_srvUavHandle, SrvParticlePosVelo0, srvUavDescriptorSize);

		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle0(m_srvUavHandle, SrvParticlePosVelo0, srvUavDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle1(m_srvUavHandle, SrvParticlePosVelo1, srvUavDescriptorSize);
		device->CreateShaderResourceView(m_particleBuffer0.Get(), &srvDesc, srvHandle0);
		device->CreateShaderResourceView(m_particleBuffer1.Get(), &srvDesc, srvHandle1);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = ParticleCount;
		uavDesc.Buffer.StructureByteStride = sizeof(Particle);
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE uavHandle0(m_srvUavHandle, UavParticlePosVelo0, srvUavDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE uavHandle1(m_srvUavHandle, UavParticlePosVelo1, srvUavDescriptorSize);
		device->CreateUnorderedAccessView(m_particleBuffer0.Get(), nullptr, &uavDesc, uavHandle0);
		device->CreateUnorderedAccessView(m_particleBuffer1.Get(), nullptr, &uavDesc, uavHandle1);

		ThrowIfFailed(iList->Close(), R_GetGpuError);
		ID3D12CommandList* ppList[] = { iList };
		r_graphicsCard[0]->ExecuteSync(_countof(ppList), ppList);

		GraphicsContext::Push(commandList, commandAllocator);
	}

	SampleParticles::~SampleParticles()
	{
		delete m_vertexBuffer;
	}

	uint32_t SampleParticles::GetParticleCount()
	{
		return ParticleCount;
	}

	ID3D12DescriptorHeap* SampleParticles::GetDescriptorHeap()
	{
		return m_srvUavHeap.Get();
	}

	tuple<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> SampleParticles::GetSrvHandle()
	{
		return { m_srvUavHandle, m_srvUavGpuHandle };
	}

	VertexBuffer* SampleParticles::GetVertexBuffer()
	{
		return m_vertexBuffer;
	}

	ID3D12Resource* SampleParticles::GetSrvUav()
	{
		if (m_srvIndex == 0) return m_particleBuffer0.Get();
		else return m_particleBuffer1.Get();
	}

	void SampleParticles::SwapSrvUavIndex()
	{
		m_srvIndex = 1 - m_srvIndex;
	}

	std::tuple<uint32_t, uint32_t, ID3D12Resource*> SampleParticles::GetSrvIndexAndUavResource()
	{
		if (m_srvIndex == 0)
		{
			return { SrvParticlePosVelo0, UavParticlePosVelo1, m_particleBuffer1.Get() };
		}
		else
		{
			return { SrvParticlePosVelo1, UavParticlePosVelo0, m_particleBuffer0.Get() };
		}
	}

	void SampleParticles::InitializeParticles(Particle* _pParticles, const XMFLOAT3& _center, const XMFLOAT4& _velocity, float _spread,
		uint32_t _particlesNum)
	{
		Randomize();
		for (uint32_t i = 0; i < _particlesNum; i++)
		{
			XMFLOAT3 delta(_spread, _spread, _spread);

			while (XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&delta))) > _spread* _spread)
			{
				delta.x = RandomPercent() * _spread;
				delta.y = RandomPercent() * _spread;
				delta.z = RandomPercent() * _spread;
			}

			_pParticles[i].position.x = _center.x + delta.x;
			_pParticles[i].position.y = _center.y + delta.y;
			_pParticles[i].position.z = _center.z + delta.z;
			_pParticles[i].position.w = 10000.0f * 10000.0f;

			_pParticles[i].velocity = _velocity;
		}
	}

	float SampleParticles::RandomPercent()
	{
		float ret = static_cast<float>((rand() % 10000) - 5000);
		return ret / 5000.0f;
	}
}