#pragma once
#ifndef __OBJECTBEHAVIOUR_H__
#define __OBJECTBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"GameObject.h"
#include"ComponentBehaviour.h"

namespace AEngine::Game
{
	class Scene;

	class ObjectBehaviour : public BaseBehaviour, public GameObject
	{
		bool m_active;
		Scene* m_scene;
		std::mutex m_mutex;

		// 当前物体的一些组件，比如渲染器、脚本等等。
		// Components of this object, such script、renderer、rigidbody etc.
		std::vector<ComponentBehaviour*> m_component;

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

		std::vector<ObjectBehaviour*> GetComponents();
		template<typename T = ComponentBehaviour>
		T* GetComponentByName(string name)
		{
			for (var i : m_component)
			{
				if (i->name == name)
					return i;
			}
		}

		void AddComponent(ComponentBehaviour* component);
		void RemoveComponentByName(string name);

		// 物体刚刚被加入场景时调用
		// Call when the object add to scene
		virtual void Start();
		// 物体为激活时调用
		// Call when the object is actived
		virtual void OnActive();
		// 更新时调用
		// Call when pdate
		virtual void Update();
		//virtual void FixedUpdate();

		// 物体未被激活但还在场景中时调用
		// Call when object is not actived but in scene
		virtual void OnInvalid();
		// 物体被删除时调用（同时从场景中删除）
		// Call when object is delete from scene
		virtual void Destory();

		bool Active();
		void Active(bool b);
	};
}

#endif // !__OBJECTBEHAVIOUR_H__
