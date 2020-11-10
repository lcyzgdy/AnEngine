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
		void InitAsConstants(uint32_t Register, uint32_t numDwords, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsConstantBuffer(uint32_t Register, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsBufferSRV(uint32_t Register, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsBufferUAV(uint32_t Register, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, uint32_t Register, uint32_t count, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsDescriptorTable(uint32_t rangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void SetTableRange(uint32_t rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE type, uint32_t Register, uint32_t count, uint32_t space = 0);

		const D3D12_ROOT_PARAMETER& operator() (void) const { return m_rootParam; }
	};

	class RootSignature
	{
	protected:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature_cp;

	public:
		explicit RootSignature(ID3D12RootSignature* rootSignature);
		RootSignature() = delete;
		RootSignature(const RootSignature&) = delete;
		~RootSignature() = default;
		void Reset();

		ID3D12RootSignature* GetRootSignature() { return m_rootSignature_cp.Get(); }
		ID3D12RootSignature** operator&() throw() { return &m_rootSignature_cp; }
	};
}

#endif // !__ROOTSIGNATURE_H__