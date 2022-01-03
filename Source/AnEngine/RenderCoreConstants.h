#pragma once
#ifndef __RENDERCORECONSTANTS_H__
#define __RENDERCORECONSTANTS_H__

#include"DX.h"

namespace AnEngine::RenderCore
{
	static const uint32_t r_DefaultFrameCount_const = 2;
	static const uint32_t r_SwapChainBufferCount_const = r_DefaultFrameCount_const;
	static const uint32_t r_DefaultThreadCount_const = 1;
	static const DXGI_FORMAT r_DefaultSwapChainFormat_const = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const float r_ClearColor_const_float[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const bool r_IsUseWarpDevice_const = false;
	static const D3D_FEATURE_LEVEL r_MinD3DFeatureLevel_const = D3D_FEATURE_LEVEL_11_0;
	static const D3D_FEATURE_LEVEL r_D3DFeatureLevelWithCreatorUpdate_const = D3D_FEATURE_LEVEL_12_1;

	enum EngineType
	{
		Cpu,
		Render,
		Cpoy,
		Compute,
		Encode,
		Decode
	};

	enum class RaytracingAPI {
		FallbackLayer,
		DirectXRaytracing,
	};

	static const RaytracingAPI r_RaytracingAPI = RaytracingAPI::FallbackLayer;
#define RAYTRACING_API_FALLBACK_LAYER
	//#undef RAYTRACING_API_DIRECTX_RAYTRACING;
}

#endif // !__RENDERCORECONSTANTS_H__
