#pragma once
#ifndef __RAYTRACINGPIPELINE_H__
#define __RAYTRACINGPIPELINE_H__

#include "RayTracingHelper.hpp"

namespace AnEngine::RenderCore
{
	class RayTracingPipeline
	{
		Microsoft::WRL::ComPtr<ID3D12CommandListRaytracingPrototype> m_dxrCommandList;
		Microsoft::WRL::ComPtr<ID3D12StateObjectPrototype> m_dxrStateObject;
	public:
		RayTracingPipeline() = default;

		void Initialize();
	};
}

#endif // __RAYTRACINGPIPELINE_H__
