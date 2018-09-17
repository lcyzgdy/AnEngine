#include "RootSignature.h"
#include "RenderCore.h"
#include <functional>

using namespace Microsoft::WRL;

namespace AnEngine::RenderCore
{
	RootParameter::RootParameter()
	{
		m_rootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	RootParameter::~RootParameter()
	{
		Clear();
	}

	void RootParameter::Clear()
	{
		if (m_rootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			delete[] m_rootParam.DescriptorTable.pDescriptorRanges;

		m_rootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	void RootParameter::InitAsConstants(uint32_t Register, uint32_t numDwords, D3D12_SHADER_VISIBILITY visibility)
	{
		m_rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		m_rootParam.ShaderVisibility = visibility;
		m_rootParam.Constants.Num32BitValues = numDwords;
		m_rootParam.Constants.ShaderRegister = Register;
		m_rootParam.Constants.RegisterSpace = 0;
	}

	void RootParameter::InitAsConstantBuffer(uint32_t Register, D3D12_SHADER_VISIBILITY visibility)
	{
		m_rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		m_rootParam.ShaderVisibility = visibility;
		m_rootParam.Descriptor.ShaderRegister = Register;
		m_rootParam.Descriptor.RegisterSpace = 0;
	}

	void RootParameter::InitAsBufferSRV(uint32_t Register, D3D12_SHADER_VISIBILITY visibility)
	{
		m_rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		m_rootParam.ShaderVisibility = visibility;
		m_rootParam.Descriptor.ShaderRegister = Register;
		m_rootParam.Descriptor.RegisterSpace = 0;
	}

	void RootParameter::InitAsBufferUAV(uint32_t Register, D3D12_SHADER_VISIBILITY visibility)
	{
		m_rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		m_rootParam.ShaderVisibility = visibility;
		m_rootParam.Descriptor.ShaderRegister = Register;
		m_rootParam.Descriptor.RegisterSpace = 0;
	}

	void RootParameter::InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t count, D3D12_SHADER_VISIBILITY visibility)
	{
		InitAsDescriptorTable(1, visibility);
		SetTableRange(0, Type, Register, count);
	}

	void RootParameter::InitAsDescriptorTable(uint32_t rangecount, D3D12_SHADER_VISIBILITY visibility)
	{
		m_rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		m_rootParam.ShaderVisibility = visibility;
		m_rootParam.DescriptorTable.NumDescriptorRanges = rangecount;
		m_rootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[rangecount];
	}

	void RootParameter::SetTableRange(uint32_t rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t count, uint32_t space)
	{
		D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(m_rootParam.DescriptorTable.pDescriptorRanges + rangeIndex);
		range->RangeType = Type;
		range->NumDescriptors = count;
		range->BaseShaderRegister = Register;
		range->RegisterSpace = space;
		range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}
}

namespace AnEngine::RenderCore
{


	RootSignature::RootSignature(ID3D12RootSignature* rootSignature)
	{
		var device = r_graphicsCard[0]->GetDevice();
		/*
		CD3DX12_DESCRIPTOR_RANGE1 range[1];
		range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

		CD3DX12_ROOT_PARAMETER1 params[1];
		params[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_ALL);
		*/

		//CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		//rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		//InitParams(rootSignatureDesc);

		/*ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));

		ThrowIfFailed(device->CreateRootSignature(r_graphicsCard[0]->GetNodeNum(), signature->GetBufferPointer(),
			signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature_cp)));*/
		m_rootSignature_cp.Attach(rootSignature);
	}

	void RootSignature::Reset()
	{
	}
}