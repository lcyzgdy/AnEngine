#include "Renderer.h"
#include"CommandContext.h"
#include"RenderCore.h"
#include"GameObject.h"

namespace AnEngine::Game
{
	void Renderer::Start()
	{
		LoadAsset();
	}

	void Renderer::BeforeUpdate()
	{
		m_renderTarget = nullptr;
	}

	void Renderer::Update()
	{
		if (m_renderTarget != nullptr)
		{
			OnRender();
		}
	}

	void Renderer::AfterUpdate()
	{
	}

	void TrangleRender::LoadAsset()
	{
	}

	void TrangleRender::OnRender()
	{

	}
}