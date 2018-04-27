#pragma once
#ifndef __OBJECTBEHAVIOUR_H__
#define __OBJECTBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"GameObject.h"
#include<condition_variable>
//#include"ComponentBehaviour.h"

namespace AnEngine::Game
{
	class Scene;

	class ObjectBehaviour : public BaseBehaviour//, public GameObject
	{
		friend class Scene;
		friend class GameObject;

		// 通过 BaseBehaviour 继承
		virtual void OnInit() final;
		// 更新前调用，如没有动作可忽略
		// Call before update, ignore if do nothing
		virtual void BeforeUpdate() final;
		// 更新调用
		// Call update, ignore if do nothing
		virtual void OnUpdate() final;
		// 更新后调用
		// Call after update, ignore if do nothing
		virtual void AfterUpdate() final;
		virtual void OnRelease() final;

	protected:
		bool m_active;
		Scene* m_scene;

		GameObject* gameObject;

		// 当前物体的一些组件，比如渲染器、脚本等等。
		// Components of this object, such script、renderer、rigidbody etc.
		// std::vector<ObjectBehaviour*> m_component;

	protected:
		// 物体刚刚被加入场景时调用
		// Call when the object add to scene
		virtual void Start();

		// 物体为激活时调用
		// Call when the object is actived
		virtual void OnActive();

		// 更新时调用，暴露
		// Call when update
		virtual void Update();
		//virtual void FixedUpdate();

		// 更新后调用，暴露
		virtual void LateUpdate();

		// 物体未被激活但还在场景中时调用
		// Call when object is not actived but in scene
		virtual void OnInvalid();

		// 物体被删除时调用（同时从场景中删除）
		// Call when object is delete from scene
		virtual void Destory();

	public:
		//ObjectBehaviour(const std::wstring& name);
		//ObjectBehaviour(std::wstring&& name);
		ObjectBehaviour();
		virtual ~ObjectBehaviour();

		bool Active();
		void Active(bool b);
	};

	using Script = ObjectBehaviour;
}

#endif // !__OBJECTBEHAVIOUR_H__
