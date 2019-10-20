#include "DeferredRenderPipeline.h"
#include "SceneManager.h"
#include "MeshRendererComponent.h"
#include "Lighting.h"

using namespace std;
using namespace AnEngine::Game;
using namespace AnEngine::RenderCore;

namespace AnEngine::RenderCore
{
	void DeferredRenderPipeline::OnRender(std::mutex& sceneResMutex)
	{
		FenceContext::Instance()->WaitAllFence();

		/* GBuffer阶段，渲染所有摄像机的 GBuffer、Depth、Normal */
		//lock_guard<mutex> lock(sceneResMutex);
		GBuffer();

		/* 对于场景中的每一个光源，绘制其深度图，以生成阴影。 */
		//lock_guard<mutex> lock(sceneResMutex);
		DepthPreLight();

		/* 阴影 */
		FenceContext::Instance()->WaitAllFence();
		Shadow();

		/* 反射 */

		/* 使用前向渲染处理半透明物体 */

		/* 天空盒 */

		/* 全屏后处理 */
		FenceContext::Instance()->WaitAllFence();
		PostProcess();

		/* 绘制UI组件 */
		UI();
	}

	void DeferredRenderPipeline::GBuffer()
	{
		var scene = Game::SceneManager::ActiveScene();
		// 网格渲染器是 ECS 组织的
		//var objs = scene->GetGroupOfType<Component::MeshRenderer>();	// 从场景中获取所有的 Mesh Renderer 组件进行渲染
		//const auto& mrs = *objs;
		var[list, allocator] = GraphicsContext::GetOne();
		var ilist = list->GetCommandList();
		var iallocator = allocator->GetAllocator();
		ThrowIfFailed(iallocator->Reset());

		ThrowIfFailed(ilist->Close());
		GraphicsContext::Push(list, allocator);
	}

	void DeferredRenderPipeline::DepthPreLight()
	{
		var scene = Game::SceneManager::ActiveScene();
		// 灯光是 Behaviour 组织的

	}

	void DeferredRenderPipeline::Shadow()
	{
	}

	void DeferredRenderPipeline::UI()
	{
	}
	void DeferredRenderPipeline::PostProcess()
	{
	}
}