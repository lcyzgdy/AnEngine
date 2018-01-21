#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include"ObjectBehaviour.h"

namespace AnEngine::Game
{
	class Renderer : public ObjectBehaviour
	{
	public:
		Renderer();
		~Renderer();

		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;
	};
}

#endif // !__RENDERER_H__
