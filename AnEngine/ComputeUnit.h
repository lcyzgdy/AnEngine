#pragma once
#ifndef __COMPUTEUNIT_H__
#define __COMPUTEUNIT_H__

#include"DX.h"
#include"PipelineState.h"
#include"ShaderClass.h"

namespace AnEngine::Game
{
	class ComputeUnit
	{
		RenderCore::ComputePSO* m_pso;
		RenderCore::RootSignature* m_rootSignature;

	public:
		ComputeUnit();

		void Dispatch(uint32_t threadGroupX, uint32_t threadGroupY, uint32_t threadGroupZ);
	};
}

#endif // !__COMPUTEUNIT_H__
