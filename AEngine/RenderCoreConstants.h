#pragma once
#ifndef __RENDERCORECONSTANTS_H__
#define __RENDERCORECONSTANTS_H__

#include"DX.h"

namespace AEngine::RenderCore
{
	static const UINT r_cnt_DefaultFrameCount = 2;
	static const UINT r_cnt_SwapChainBufferCount = 3;
	static constexpr UINT r_cnt_DefaultThreadCount = 1;
	static const DXGI_FORMAT r_cnt_DefaultSwapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
	static const float r_cnt_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const bool r_cnt_IsUseWarpDevice = false;
	static const D3D_FEATURE_LEVEL r_cnt_MinD3DFeatureLevel = D3D_FEATURE_LEVEL_12_0;
	static const D3D_FEATURE_LEVEL r_cnt_D3DFeatureLevelWithCreatorUpdate = D3D_FEATURE_LEVEL_12_1;
}

#endif // !__RENDERCORECONSTANTS_H__
