#include "Engine.h"
#include "RenderCore.h"
#include "Screen.h"
#include "Input.h"
#include "ThreadPool.hpp"
#include "DTimer.h"
#include "SceneManager.h"
#include "IParallel.h"

using namespace std;
using namespace AnEngine::Game;

namespace AnEngine
{
	void Engine::UpdateBottom()
	{
		BaseInput::GetInstance()->ZeroInputState();
		if (m_running)
		{
			Utility::ThreadPool::Commit(bind(&Engine::UpdateSystem, this));
		}
	}

	void Engine::UpdateSystem()
	{
		DTimer::GetInstance()->Tick(nullptr);
		BaseInput::GetInstance()->Update();
		var scene = SceneManager::ActiveScene();

		// 执行 Behaviour 的 BeforeUpdate，这是在 System 更新之前调用的
		for (var obj : scene->GetAllGameObject())
		{
			for (var b : obj->GetAllBehaviours())
			{
				b->BeforeUpdate();
			}
		}

		for (var sys : scene->GetAllSystems())
		{
			if (is_base_of<IParallel, decltype(*sys)>::value)
			{
				var parallel = (IParallel*)sys;
				condition_variable cv;
				for (int i = 0; i < parallel->Length; i++)
				{
					Utility::ThreadPool::Commit(bind(&IParallel::Execute, parallel, i));
				}
			}
		}
		Utility::ThreadPool::Commit(bind(&Engine::UpdateBehaviour, this));
	}

	void Engine::UpdateBehaviour()
	{
		var scene = Game::SceneManager::ActiveScene();
		for (var obj : scene->GetAllGameObject())
		{
			for (var b : obj->GetAllBehaviours())
			{
				b->OnUpdate();
			}
		}
		for (var obj : scene->GetAllGameObject())
		{
			for (var b : obj->GetAllBehaviours())
			{
				b->AfterUpdate();
			}
		}
		/*for (var& obj : activeScene->GetAllGameObject())
		{
			if (!obj.Active()) continue;
			for (var be : obj.GetComponents())
			{
				be->UpdateSystem();
			}
			for (var be : obj.GetComponents())
			{
				be->Update();
			}
			for (var be : obj.GetComponents())
			{
				be->UpdateBottom();
			}
		}*/
		Utility::ThreadPool::Commit(bind(&Engine::UpdateBottom, this));
	}

	/*Engine* Engine::GetInstance()
	{
		static Engine driver;
		return &driver;
	}*/

	void Engine::Initialize(HWND hwnd, HINSTANCE hInstance, int screenw, int screenh)
	{
		if (m_initialized) return;
		m_initialized = true;
		BaseInput::GetInstance()->Initialize(hwnd, hInstance);
		Screen::GetInstance()->Initialize(screenw, screenh);
		RenderCore::InitializeRender(hwnd);
	}

	void Engine::Release()
	{
		m_initialized = false;
		m_running = false;
		SceneManager::ActiveScene()->OnUnload();
		BaseInput::GetInstance()->Release();
	}

	void Engine::StartScene()
	{
		m_running = true;
		var scene = SceneManager::ActiveScene();
		scene->OnLoad();
		Utility::ThreadPool::Commit(bind(&Engine::UpdateBottom, this));
	}
}
