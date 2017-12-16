#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include"BaseBehaviour.h"

namespace AEngine::Game
{
	class Renderer : public BaseBehaviour
	{
		bool m_inView;

		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void BeforeUpdate() override;
		virtual void OnUpdate() override;
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;
	};
}

#endif // !__RENDERER_H__
