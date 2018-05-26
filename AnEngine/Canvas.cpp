#include "Canvas.h"
#include "ThreadPool.hpp"
#include "UIComponent.h"

namespace AnEngine::Game::UI
{
	void Canvas::Start()
	{

	}

	void Canvas::LateUpdate()
	{
		gameObject->GetComponents<UIComponent>();
	}
}