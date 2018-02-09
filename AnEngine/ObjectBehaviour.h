#pragma once
#ifndef __OBJECTBEHAVIOUR_H__
#define __OBJECTBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"GameObject.h"
//#include"ComponentBehaviour.h"

namespace AnEngine::Game
{
	class Scene;

	class ObjectBehaviour : public BaseBehaviour, public GameObject
	{
		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void OnUpdate() override;
		virtual void OnRelease() override;
		//virtual void BeforeUpdate() override;
		//virtual void AfterUpdate() override;

	protected:
		bool m_active;
		Scene* m_scene;
		std::recursive_mutex m_recursiveMutex;

		// 当前物体的一些组件，比如渲染器、脚本等等。
		// Components of this object, such script、renderer、rigidbody etc.
		std::vector<ObjectBehaviour*> m_component;

		//virtual void BeginUpdate();

	protected:
		// 物体刚刚被加入场景时调用
		// Call when the object add to scene
		virtual void Start();

		// 物体为激活时调用
		// Call when the object is actived
		virtual void OnActive();

		// 更新前调用，如没有动作可忽略
		// Call before update, ignore it if do nothing
		virtual void BeforeUpdate();

		// 更新时调用
		// Call when update
		virtual void Update();
		//virtual void FixedUpdate();

		// 更新后调用
		// Call after update, ignore it if do nothing
		virtual void AfterUpdate();

		// 物体未被激活但还在场景中时调用
		// Call when object is not actived but in scene
		virtual void OnInvalid();

		// 物体被删除时调用（同时从场景中删除）
		// Call when object is delete from scene
		virtual void Destory();

	public:
		ObjectBehaviour(std::wstring name);
		~ObjectBehaviour() = default;

		std::vector<ObjectBehaviour*> GetComponents();
		template<typename T = ObjectBehaviour>
		T* GetComponentByName(string name)
		{
			for (var i : m_component)
			{
				if (i->name == name)
					return i;
			}
		}

		template<typename T = ObjectBehaviour>
		T GetComponent()
		{
			for (var i : m_component)
			{
				if (dynamic_cast<T*>(i) != nullptr)
					return i;
			}
		}

		void AddComponent(ObjectBehaviour* component);
		void RemoveComponent(ObjectBehaviour* component);

		bool Active();
		void Active(bool b);
	};
}

#endif // !__OBJECTBEHAVIOUR_H__
