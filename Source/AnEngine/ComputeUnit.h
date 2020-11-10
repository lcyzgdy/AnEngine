#pragma once
#ifndef __COMPUTEUNIT_H__
#define __COMPUTEUNIT_H__

#include"DX.h"
#include"PipelineState.h"
#include"ShaderClass.h"
#include"Fence.h"

namespace AnEngine::Game
{
	class ComputeUnit
	{
		RenderCore::ComputePSO* m_pso;
		RenderCore::RootSignature* m_rootSignature;
		RenderCore::ComputeShader* m_cs;

		RenderCore::Fence* m_fence;

	public:
		ComputeUnit(std::wstring&& csFileName);
		ComputeUnit(const std::wstring& csFileName);
		~ComputeUnit();

		void Dispatch(uint32_t threadGroupX, uint32_t threadGroupY, uint32_t threadGroupZ);
	};
}

#endif // !__COMPUTEUNIT_H__
