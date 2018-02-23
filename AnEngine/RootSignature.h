#pragma once
#ifndef __ROOTSIGNATURE_H__
#define __ROOTSIGNATURE_H__

#include"DX.h"

namespace AnEngine::RenderCore
{
	class RootParameter
	{
		friend class RootSignature;

	protected:
		D3D12_ROOT_PARAMETER m_rootParam;

	public:

		RootParameter();
		~RootParameter();
		void Clear();
		void InitAsConstants(uint32_t Register, uint32_t NumDwords, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsConstantBuffer(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsBufferSRV(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsBufferUAV(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t Count, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsDescriptorTable(uint32_t RangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void SetTableRange(uint32_t RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t Count, uint32_t Space = 0);

		const D3D12_ROOT_PARAMETER& operator() (void) const { return m_rootParam; }
	};

	class RootSignature
	{
	protected:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature_cp;

	public:
		RootSignature();
		~RootSignature() = default;
		void Reset();

		ID3D12RootSignature* GetRootSignature();
		ID3D12RootSignature** operator&() throw();
	};
}

#endif // !__ROOTSIGNATURE_H__