#pragma once
#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "onwind.h"
#include "Scene.h"

namespace AnEngine::Game
{
	static class SceneManager// : public ::Singleton<SceneManager>
	{
		static std::map<wstring, Scene*> m_scenes;
		static Scene* m_activeScene;
	public:
		static void AddNewScene(Scene* scene);
		static void LoadScene(const wstring& sceneName);
		static void LoadScene(wstring&& sceneName);
		static Scene* ActiveScene() { return m_activeScene; }
		static std::vector<Scene*> GetAllScenes();
	};
}

#endif // !__SCENEMANAGER_H__
