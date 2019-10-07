#include "SceneManager.h"

using namespace std;

namespace AnEngine::Game
{
	void SceneManager::AddNewScene(Scene* scene)
	{
		var sm = Instance();
		if (sm->m_scenes.empty()) sm->m_activeScene = scene;
		sm->m_scenes[scene->name] = scene;
	}

	void SceneManager::LoadScene(const string& sceneName)
	{
		var sm = Instance();
		if (sm->m_scenes.count(sceneName) == 0) throw exception("No scene named ");
		if (sm->m_activeScene->name == sceneName)
		{
			throw exception("Load same scene");
		}
		sm->m_activeScene->onUnload();
		sm->m_activeScene = sm->m_scenes[sceneName];
		sm->m_activeScene->onLoad();
	}

	void SceneManager::LoadScene(string&& sceneName)
	{
		var sm = Instance();
		if (sm->m_scenes.count(sceneName) == 0) throw exception("No scene named ");
		if (sm->m_activeScene->name == sceneName)
		{
			throw exception("Load same scene");
		}
		sm->m_activeScene->onUnload();
		sm->m_activeScene = sm->m_scenes[sceneName];
		sm->m_activeScene->onLoad();
	}

	vector<Scene*> SceneManager::GetAllScenes()
	{
		vector<Scene*> res;
		res.reserve(32);
		for (var i : Instance()->m_scenes)
		{
			res.push_back(i.second);
		}
		return res;
	}
}