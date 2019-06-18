#pragma once
#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "onwind.h"
#include "Scene.h"

namespace AnEngine::Game
{
	class SceneManager : public Singleton<SceneManager>
	{
		//friend class Singleton<SceneManager>;
		std::map<std::wstring, Scene*> m_scenes;
		Scene* m_activeScene;
	public:
		static void AddNewScene(Scene* scene);
		static void LoadScene(const std::wstring& sceneName);
		static void LoadScene(std::wstring&& sceneName);
		static Scene* ActiveScene() { return Instance()->m_activeScene; }
		static std::vector<Scene*> GetAllScenes();
	};
}

#endif // !__SCENEMANAGER_H__
