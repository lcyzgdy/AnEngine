#include "SceneManager.h"

using namespace std;

namespace AnEngine::Game
{
	void SceneManager::AddNewScene(Scene* scene)
	{
		if (m_scenes.empty()) m_activeScene = scene;
		m_scenes[scene->name] = scene;
	}

	void SceneManager::LoadScene(const wstring& sceneName)
	{
		if (m_scenes.count(sceneName) == 0) throw exception("No scene named ");
		if (m_activeScene->name == sceneName)
		{
			throw exception("Load same scene");
		}
		m_activeScene->OnUnload();
		m_activeScene = m_scenes[sceneName];
		m_activeScene->OnLoad();
	}

	void SceneManager::LoadScene(wstring&& sceneName)
	{
		if (m_scenes.count(sceneName) == 0) throw exception("No scene named ");
		if (m_activeScene->name == sceneName)
		{
			throw exception("Load same scene");
		}
		m_activeScene->OnUnload();
		m_activeScene = m_scenes[sceneName];
		m_activeScene->OnLoad();
	}

	vector<Scene*> SceneManager::GetAllScenes()
	{
		vector<Scene*> res;
		res.reserve(32);
		for (var i : m_scenes)
		{
			res.push_back(i.second);
		}
		return res;
	}
}