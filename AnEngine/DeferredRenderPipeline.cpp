#include "DeferredRenderPipeline.h"
#include "SceneManager.h"
#include "MeshRendererComponent.h"

using namespace std;
using namespace AnEngine::Game;
using namespace AnEngine::RenderCore;

namespace AnEngine::RenderCore
{
	void DeferredRenderPipeline::OnRender(std::mutex& sceneResMutex)
	{
		FenceContext::Instance()->WaitAllFence();
		{
			lock_guard<mutex> lock(sceneResMutex);
			GBuffer();
		}
		{
			lock_guard<mutex> lock(sceneResMutex);
			DepthPreLight();
		}

		{
			UI();
		}
	}

	void DeferredRenderPipeline::GBuffer()
	{
		var scene = Game::SceneManager::ActiveScene();
		var objs = scene->GetGroupOfType<Component::MeshRenderer>();
		const auto& mrs = *objs;
		var[list, allocator] = GraphicsContext::GetOne();
		var ilist = list->GetCommandList();
		var iallocator = allocator->GetAllocator();
		ThrowIfFailed(iallocator->Reset());


		GraphicsContext::Push(list, allocator);
	}

	void DeferredRenderPipeline::DepthPreLight()
	{
	}

	void DeferredRenderPipeline::Shadow()
	{
	}

	void DeferredRenderPipeline::UI()
	{
	}
}