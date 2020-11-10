#pragma once
#ifndef __CANVAS_H__
#define __CANVAS_H__

#include "ObjectBehaviour.h"
#include "UIComponent.h"

namespace AnEngine::Game::UI
{
	class DLL_API Canvas : public ObjectBehaviour
	{
	protected:
		virtual void Start() override;
		virtual void LateUpdate() override;
	public:

	};
}

#endif // !__CANVAS_H__
