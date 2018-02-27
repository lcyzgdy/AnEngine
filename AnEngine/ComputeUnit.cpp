#include "ComputeUnit.h"
#include "CommandContext.h"
using namespace AnEngine;
using namespace AnEngine::RenderCore;
using namespace AnEngine::Game;

namespace AnEngine::Game
{
	void ComputeUnit::Dispatch(uint32_t threadGroupX, uint32_t threadGroupY, uint32_t threadGroupZ)
	{
		var commandList = GraphicsCommandContext::GetInstance()->GetOne();
		
	}
}