#pragma once
#ifndef __COMPONENTBEHAVIOUR_H__
#define __COMPONENTBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"GameObject.h"

namespace AEngine::Game
{
	// 组件的特点是不能脱离物体单独存在。
	class ComponentBehaviour : public BaseBehaviour
	{
		// 组件所依附的物体
		const GameObject* m_refObject;

		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void BeforeUpdate() override;
		virtual void OnUpdate() override;
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;
	};
}

#endif // !__COMPONENTBEHAVIOUR_H__
