#pragma once
#ifndef __SAMPLEPARTICLES_H__
#define __SAMPLEPARTICLES_H__

#include "VertexBuffer.h"

namespace AnEngine::RenderCore::Resource
{
	class SampleParticles
	{
		static const uint32_t ParticleCount = 12000;
		const const float ParticleSpread = 400.0f;

		struct Particle
		{
			DirectX::XMFLOAT4 position;
			DirectX::XMFLOAT4 velocity;
		};

		struct ParticleVertex
		{
			DirectX::XMFLOAT4 color;
		};

		RenderCore::Resource::VertexBuffer* m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_particleBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_particleBufferUpload;

		ID3D12DescriptorHeap* m_srvUavHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_srvUavHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_srvUavGpuHandle;

		void SampleParticles::InitializeParticles(Particle* _pParticles, const DirectX::XMFLOAT3& _center, const DirectX::XMFLOAT4& _velocity,
			float _spread, uint32_t _particlesNum);
		float SampleParticles::RandomPercent();

	public:
		SampleParticles();
		~SampleParticles();

		uint32_t GetParticleCount();
		ID3D12DescriptorHeap* GetDescriptorHeap();
		std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> GetHandle();
		VertexBuffer* GetVertexBuffer();
		ID3D12Resource* GetSrvUav();



		enum DescriptorHeapIndex : uint32_t
		{
			UavParticlePosVelo0,
			SrvParticlePosVelo0,
			DescriptorCount
		};	// 着色器资源的索引
	};
}

#endif // !__SAMPLEPARTICLES_H__
