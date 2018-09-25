#include "SceneManager.h"

using namespace std;

namespace AnEngine::Game
{
	void SceneManager::AddNewScene(Scene* scene)
	{
		m_scenes[scene->name] = scene;
	}

	void SceneManager::LoadScene(const wstring& sceneName)
	{
		if (m_scenes.count(sceneName) == 0) throw exception("No scene named ");
		if (m_activeScene->name == sceneName)
		{
			throw exception("Load same scene");
		}
		m_activeScene = m_scenes[sceneName];
	}

	void SceneManager::LoadScene(wstring&& sceneName)
	{
		if (m_scenes.count(sceneName) == 0) throw exception("No scene named ");
		if (m_activeScene->name == sceneName)
		{
			throw exception("Load same scene");
		}
		m_activeScene = m_scenes[sceneName];
	}

	vector<Scene*> SceneManager::GetAllScenes()
	{
		vector<Scene*> res;
		for (var i : m_scenes)
		{
			res.push_back(i.second);
		}
		return res;
	}
}