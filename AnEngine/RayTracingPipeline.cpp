#include "RayTracingPipeline.h"
#include "GraphicsCard.h"
#include "RenderCoreConstants.h"
#include "../Assets/CompiledShaders/Raytracing.hlsl.h"

namespace AnEngine::RenderCore
{
	void RayTracingPipeline::Initialize()
	{
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

		const wchar_t* hitGroupName = L"MyHitGroup";
		const wchar_t* raygenShaderName = L"MyRaygenShader";
		const wchar_t* closestHitShaderName = L"MyClosestHitShader";
		const wchar_t* missShaderName = L"MyMissShader";

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
		auto hitGroup = raytracingPipeline.CreateSubobject<CD3D12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetClosestHitShaderImport(closestHitShaderName);
		hitGroup->SetHitGroupExport(hitGroupName);

		// Shader config
		// Defines the maximum sizes in bytes for the ray payload and attribute structure.
		auto shaderConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		UINT payloadSize = 4 * sizeof(float);   // float4 color
		UINT attributeSize = 2 * sizeof(float); // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);

		// Local root signature and shader association
		//CreateLocalRootSignatureSubobjects(&raytracingPipeline);
		// This is a root signature that enables a shader to have unique arguments that come from shader tables.

		// Global root signature
		// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3D12_ROOT_SIGNATURE_SUBOBJECT>();
		//globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

		// Pipeline config
		// Defines the maximum TraceRay() recursion depth.
		auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		// PERFOMANCE TIP: Set max recursion depth as low as needed 
		// as drivers may apply optimization strategies for low recursion depths. 
		UINT maxRecursionDepth = 1; // ~ primary rays only. 
		pipelineConfig->Config(maxRecursionDepth);

#if _DEBUG
		PrintStateObjectDesc(raytracingPipeline);
#endif

		// Create the state object.
		/*if (r_raytracingAPI == RaytracingAPI::FallbackLayer)
		{
			ThrowIfFailed(r_fallbackDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_fallbackStateObject)), L"Couldn't create DirectX Raytracing state object.\n");
		}
		else // DirectX Raytracing
		{
			ThrowIfFailed(r_dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject)), L"Couldn't create DirectX Raytracing state object.\n");
		}*/
	}
}