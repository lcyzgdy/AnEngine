#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "onwind.h"
#include "BaseBehaviour.h"
#include "GameObject.h"
#include "Camera.h"
#include <condition_variable>

namespace AnEngine::Game
{
	class Scene : public BaseBehaviour//, public NonCopyable
	{
		// Scene直接调度BaseBehaviour
		friend class ::AnEngine::Engine;

		std::vector<GameObject*> m_objects;
		//std::vector<ObjectBehaviour*> m_objects;
		//Camera* defaultCamera;

		std::condition_variable m_cv;
		std::mutex m_behaviourMutex;
		uint32_t m_complateCount;

		bool m_frameLoop;

		ObjectBehaviour* m_canvas;

		//protected:
		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		// 对象状态、状态机、AI、物理等
		virtual void BeforeUpdate() override;
		// 脚本在这里运行
		virtual void OnUpdate() override;
		// 好像没什么东西了
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;

	public:
		Scene(std::wstring _name);
		~Scene() = default;

		std::wstring name;

		void AddObject(GameObject* obj);
		void RemoveObject(GameObject* obj);
	};
}

#endif // !__SCENE_H__
