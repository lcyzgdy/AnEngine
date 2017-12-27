#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include"onwind.h"
#include"BaseBehaviour.h"
#include"GameObject.h"
#include"Camera.h"

namespace AEngine::Game
{
	class Scene :public BaseBehaviour, public NonCopyable
	{
		std::vector<BaseBehaviour*> m_objects;
		Camera* defaultCamera;

		//protected:
		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void BeforeUpdate() override;
		virtual void OnUpdate() override;
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;

	public:
		Scene() = default;
		~Scene() = default;

		void AddObject(GameObject* obj);
		void RemoveObject(GameObject* obj);
	};
}

#endif // !__SCENE_H__
