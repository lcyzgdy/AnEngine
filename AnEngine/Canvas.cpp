#include "Canvas.h"
#include "ThreadPool.hpp"
#include "UIComponent.h"
#include "GraphicsCard2D.h"
#include "RenderCore.h"
using namespace AnEngine::RenderCore;

namespace AnEngine::Game::UI
{
	void Canvas::Start()
	{

	}

	void Canvas::LateUpdate()
	{
		r_graphicsCard2D->Begin();

		/*for (var each : gameObject->GetComponentsInChildren<UIComponent>())
		{
			each->OnGUI();
		}*/

		r_graphicsCard2D->End();
	}
}