#pragma once
#ifndef __OBJECTBEHAVIOUR_H__
#define __OBJECTBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"GameObject.h"

namespace AEngine::Game
{
	class ObjectBehaviour : public BaseBehaviour, public GameObject
	{
		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		//virtual void OnUpdate() override;
		//virtual void OnRender() override;
		virtual void OnRunning() override;
		virtual void OnRelease() override;

	public:
		ObjectBehaviour() = default;
		~ObjectBehaviour() = default;

		virtual void OnStart();
		virtual void OnAwake();
		virtual void OnEnable();
		virtual void OnUpdate();
		virtual void OnFixedUpdate();
		virtual void OnUnenable();
		virtual void OnDestory();
	};
}

#endif // !__OBJECTBEHAVIOUR_H__
