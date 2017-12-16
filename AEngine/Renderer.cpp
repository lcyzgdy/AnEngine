#include "Renderer.h"
#include"CommandContext.h"
#include"RenderCore.h"

namespace AEngine::Game
{
	void Renderer::OnInit()
	{
	}

	void Renderer::BeforeUpdate()
	{
	}

	void Renderer::OnUpdate()
	{
	}

	void Renderer::AfterUpdate()
	{
		var i = CommandListPool::GetInstance();
	}

	void Renderer::OnRelease()
	{
	}
}