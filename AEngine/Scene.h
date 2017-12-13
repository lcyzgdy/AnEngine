#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include"BaseBehaviour.h"
#include"GameObject.h"
#include"onwind.h"

namespace AEngine::Game
{
	class Scene :public BaseBehaviour, public NonCopyable
	{
		std::vector<GameObject*> m_objects;

		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void OnRunning() override;
		virtual void OnRelease() override;

	public:
		Scene() = default;
		~Scene() = default;
	};
}

#endif // !__SCENE_H__
