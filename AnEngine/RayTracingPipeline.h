#pragma once
#ifdef __USE_RAYTRACING__
#ifndef __RAYTRACINGPIPELINE_H__
#define __RAYTRACINGPIPELINE_H__

#include "RayTracingHelper.hpp"
#include "RaytracingHlslCompat.h"
#include "CommandList.h"

namespace AnEngine::RenderCore
{
	namespace GlobalRootSignatureParams
	{
		enum Value {
			OutputViewSlot = 0,
			AccelerationStructureSlot,
			Count
		};
	}

	namespace LocalRootSignatureParams
	{
		enum Value {
			ViewportConstantSlot = 0,
			Count
		};
	}

	class RaytracingPipeline
	{
		CommandList* m_commandList;
		CommandAllocator* m_commandAllocator[r_DefaultFrameCount_const];

		Microsoft::WRL::ComPtr<ID3D12RaytracingFallbackCommandList> m_dxrCommandList;
		Microsoft::WRL::ComPtr<ID3D12RaytracingFallbackStateObject> m_dxrStateObject;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rtGlobalRootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rtLocalRootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rtEmptyLocalRootSignature;

		// Raytracing scene
		RayGenConstantBuffer m_rayGenCB;

		// Raytracing output
		Microsoft::WRL::ComPtr<ID3D12Resource> m_raytracingOutput;
		D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
		uint32_t m_raytracingOutputResourceUAVDescriptorHeapIndex;

		// Shader table
		Microsoft::WRL::ComPtr<ID3D12Resource> m_missShaderTable;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_hitGroupShaderTable;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_rayGenShaderTable;

		void InitializeRaytracing();
		void CreateRootSignatures();
		void SerializeAndCreateRaytracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, Microsoft::WRL::ComPtr<ID3D12RootSignature>* rootSig);
		void RaytracingPipeline::CreateLocalRootSignatureSubobjects(CD3D12_STATE_OBJECT_DESC* raytracingPipeline);

	public:
		RaytracingPipeline() = default;
		~RaytracingPipeline();

		void Initialize();
	};
}

#endif // __RAYTRACINGPIPELINE_H__
#endif // __USE_RAYTRACING__
