#include "ComputeUnit.h"
#include "CommandContext.h"
#include "RenderCore.h"
using namespace AnEngine;
using namespace AnEngine::RenderCore;
using namespace AnEngine::Game;
using namespace std;

namespace AnEngine::Game
{
	ComputeUnit::ComputeUnit(wstring&& csFileName)
	{
		//m_rootSignature = new RootSignature([]( (ID3D12RootSignature**)0 ) -> {});
		m_cs = new ComputeShader(csFileName);
		m_pso = new ComputePSO();
		//m_fence = new Fence(r_graphicsCard[0]->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE));
		m_fence = new Fence();

		//m_pso->SetRootSignature(m_rootSignature->GetRootSignature());
		m_pso->SetComputeShader(m_cs->GetByteCode());
		m_pso->Finalize();
	}

	ComputeUnit::ComputeUnit(const wstring & csFileName)
	{
		//m_rootSignature = new RootSignature();
		m_pso = new ComputePSO();
		//m_fence = new Fence(r_graphicsCard[0]->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE));
		m_fence = new Fence();
	}

	ComputeUnit::~ComputeUnit()
	{
		delete m_rootSignature;
		delete m_pso;
		delete m_fence;
	}

	void ComputeUnit::Dispatch(uint32_t threadGroupX, uint32_t threadGroupY, uint32_t threadGroupZ)
	{
		var[pList, pAllocator] = ComputeContext::GetOne();
		var iList = pList->GetCommandList();
		var iAllocator = pAllocator->GetAllocator();

		iAllocator->Reset();
		iList->Reset(iAllocator, nullptr);

		iList->SetComputeRootSignature(m_rootSignature->GetRootSignature());
		iList->SetPipelineState(m_pso->GetPSO());


		iList->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
		ID3D12CommandList* ppList[] = { iList };
		r_graphicsCard[0]->ExecuteSync(_countof(ppList), ppList);
		//m_fence->GpuSignal(0);

		ComputeContext::Push(pList, pAllocator);
	}
}