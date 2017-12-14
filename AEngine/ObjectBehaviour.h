#pragma once
#ifndef __OBJECTBEHAVIOUR_H__
#define __OBJECTBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"GameObject.h"

namespace AEngine::Game
{
	class ObjectBehaviour : public BaseBehaviour, public GameObject
	{
		bool m_active;

		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void OnUpdate() override;
		virtual void OnRelease() override;
		virtual void BeforeUpdate() override;
		virtual void AfterUpdate() override;

		virtual void BeginUpdate();

	public:
		ObjectBehaviour() = default;
		~ObjectBehaviour() = default;

		virtual void Start();
		virtual void OnActive();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void OnInvalid();
		virtual void Destory();

		bool Active();
		void Active(bool b);
	};
}

#endif // !__OBJECTBEHAVIOUR_H__
