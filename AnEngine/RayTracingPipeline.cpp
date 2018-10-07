#include "RayTracingPipeline.h"

#ifdef __USE_RAYTRACING__



#include "GraphicsCard.h"
#include "RenderCoreConstants.h"
#include "RenderCore.h"
#include "CommandContext.h"
#include "../Assets/CompiledShaders/Raytracing.hlsl.h"
using namespace Microsoft::WRL;

namespace AnEngine::RenderCore
{
	const wchar_t* hitGroupName = L"MyHitGroup";
	const wchar_t* raygenShaderName = L"MyRaygenShader";
	const wchar_t* closestHitShaderName = L"MyClosestHitShader";
	const wchar_t* missShaderName = L"MyMissShader";

	void RaytracingPipeline::InitializeRaytracing()
	{
		var dxrDevice = ((GraphicsCardWithRT*)r_graphicsCard[0].get())->GetDxrDevice();
		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		m_commandList = commandList;
		for (int i = 1; i < r_DefaultFrameCount_const; i++)
		{
			m_commandAllocator[i] = new CommandAllocator();
		}

		CreateRaytracingFallbackDeviceFlags createDeviceFlags = CreateRaytracingFallbackDeviceFlags::ForceComputeFallback;
		dxrDevice->QueryRaytracingCommandList(m_commandList->GetCommandList(), IID_PPV_ARGS(&m_dxrCommandList));
	}

	void RaytracingPipeline::CreateRootSignatures()
	{
		// 全局根签名
		{
			CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
			UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
			CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
			rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &UAVDescriptor);
			rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
			CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
			SerializeAndCreateRaytracingRootSignature(globalRootSignatureDesc, &m_rtGlobalRootSignature);
		}
		// 局部根签名
		{
			CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
			rootParameters[LocalRootSignatureParams::ViewportConstantSlot].InitAsConstants(SizeOfInUint32(m_rayGenCB), 0, 0);
			CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
			localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
			SerializeAndCreateRaytracingRootSignature(localRootSignatureDesc, &m_rtLocalRootSignature);
		}
		// 空的本地根签名
		{
			CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(D3D12_DEFAULT);
			localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
			SerializeAndCreateRaytracingRootSignature(localRootSignatureDesc, &m_rtEmptyLocalRootSignature);
		}
	}

	void RaytracingPipeline::SerializeAndCreateRaytracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig)
	{
		var dxrDevice = ((GraphicsCardWithRT*)r_graphicsCard[0].get())->GetDxrDevice();
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error;

		ThrowIfFailed(dxrDevice->D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error));
		ThrowIfFailed(dxrDevice->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSig))));
	}

	void RaytracingPipeline::CreateLocalRootSignatureSubobjects(CD3D12_STATE_OBJECT_DESC* raytracingPipeline)
	{
		// Ray gen shader中所用的局部根签名
		{
			var localRootSignature = raytracingPipeline->CreateSubobject<CD3D12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignature->SetRootSignature(m_rtLocalRootSignature.Get());
			// Shader association
			var rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
			rootSignatureAssociation->AddExport(raygenShaderName);
		}
		{
			var localRootSignature = raytracingPipeline->CreateSubobject<CD3D12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignature->SetRootSignature(m_rtEmptyLocalRootSignature.Get());
			// Shader association
			var rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
			rootSignatureAssociation->AddExport(missShaderName);
			rootSignatureAssociation->AddExport(hitGroupName);
		}
	}

	RaytracingPipeline::~RaytracingPipeline()
	{
		delete m_commandList;
	}

	void RaytracingPipeline::Initialize()
	{
		var dxrDevice = ((GraphicsCardWithRT*)r_graphicsCard[0].get())->GetDxrDevice();

		InitializeRaytracing();
		CreateRootSignatures();

		// 创建7个组合成RTPSO的子对象：
		// 子对象需要通过默认或显式关联与DXIL导出（即着色器）相关联。
		// 默认关联适用于没有任何与其关联的任何相同类型的子对象的每个导出的着色器入口点。
		// 这个简单的示例使用除本地根签名子对象之外的默认着色器关联具有纯粹用于演示目的的明确关联。
		// 1 * DXIL库
		// 1 * 三角形组合
		// 1 * 着色器配置
		// 2 * 本地根签名和关联
		// 1 * 全局根签名
		// 1 * 管道配置

		CD3D12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };


		// DXIL库
		// 包含状态对象的着色器及其入口点。由于着色器不被视为子对象，因此需要通过DXIL库子对象传递它们。
		var lib = raytracingPipeline.CreateSubobject<CD3D12_DXIL_LIBRARY_SUBOBJECT>();
		D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void *)g_pRaytracing, ARRAYSIZE(g_pRaytracing));
		lib->SetDXILLibrary(&libdxil);

		//定义从库中将哪个着色器导出到表面。
		//如果没有为DXIL库子对象定义着色器导出，则所有着色器都将浮出水面。
		//在这个示例中，为了方便起见，可以省略这个示例，因为示例使用库中的所有着色器。
		{
			lib->DefineExport(raygenShaderName);
			lib->DefineExport(closestHitShaderName);
			lib->DefineExport(missShaderName);
		}

		// Triangle hit group
		// A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
		// In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
		var hitGroup = raytracingPipeline.CreateSubobject<CD3D12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetClosestHitShaderImport(closestHitShaderName);
		hitGroup->SetHitGroupExport(hitGroupName);

		// Shader config
		// Defines the maximum sizes in bytes for the ray payload and attribute structure.
		var shaderConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		uint32_t payloadSize = 4 * sizeof(float);   // float4 color
		uint32_t attributeSize = 2 * sizeof(float); // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);

		// Local root signature and shader association
		CreateLocalRootSignatureSubobjects(&raytracingPipeline);
		// This is a root signature that enables a shader to have unique arguments that come from shader tables.

		// Global root signature
		// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		var globalRootSignature = raytracingPipeline.CreateSubobject<CD3D12_ROOT_SIGNATURE_SUBOBJECT>();
		//globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

		// Pipeline config
		// Defines the maximum TraceRay() recursion depth.
		var pipelineConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		// PERFOMANCE TIP: Set max recursion depth as low as needed 
		// as drivers may apply optimization strategies for low recursion depths. 
		uint32_t maxRecursionDepth = 1; // ~ primary rays only. 
		pipelineConfig->Config(maxRecursionDepth);

#if _DEBUG
		PrintStateObjectDesc(raytracingPipeline);
#endif
		ThrowIfFailed(dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject)));
	}
}

#endif // __USE_RAYTRACING__