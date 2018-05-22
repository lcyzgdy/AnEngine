#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include"onwind.h"
#include"BaseBehaviour.h"
#include"GameObject.h"
#include"Camera.h"
#include<condition_variable>

namespace AnEngine::Game
{
	class Scene : public BaseBehaviour//, public NonCopyable
	{
		// Scene直接调度BaseBehaviour
		friend class ::AnEngine::Driver;

		std::vector<GameObject*> m_objects;
		//std::vector<ObjectBehaviour*> m_objects;
		//Camera* defaultCamera;

		std::condition_variable m_cv;
		std::mutex m_behaviourMutex;
		uint32_t m_complateCount;

		bool m_frameLoop;

		//protected:
		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void BeforeUpdate() override;
		virtual void OnUpdate() override;
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;

	public:
		Scene(std::wstring _name);
		~Scene() = default;

		std::wstring name;

		void AddObject(GameObject* obj);
		void RemoveObject(GameObject* obj);

		void Wait();
	};
}

#endif // !__SCENE_H__
