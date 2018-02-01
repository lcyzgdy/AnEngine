#pragma once
#ifndef __COMPONENTBEHAVIOUR_H__
#define __COMPONENTBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"GameObject.h"

namespace AnEngine::Game
{
	// 组件的特点是不能脱离物体单独存在。
	class ComponentBehaviour : public BaseBehaviour
	{
		friend class ObjectBehaviour;

	protected:
		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void BeforeUpdate();
		virtual void OnUpdate() override;
		virtual void Update();
		virtual void AfterUpdate();
		virtual void OnRelease() override;

	protected:
		// 组件所依附的物体
		GameObject * m_refObject;
		bool m_enable;

		ComponentBehaviour(bool enable);

		virtual void OnEnable();
		virtual void OnDisable();
		void SetGameObject(GameObject* gameObject);

	public:
		void Enable(bool enable);
	};
}

#endif // !__COMPONENTBEHAVIOUR_H__
