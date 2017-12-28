#include "Test.h"
#include"Driver.h"
using namespace AEngine;
using namespace AEngine::Game;

Scene* testScene;
Camera* testCamera;

void LoadScene()
{
	testScene = new Scene();
	testCamera = new Camera();
	testCamera->ClearFlag();
	TestCamera* camera = new TestCamera();
	camera->AddComponent(testCamera);
	testScene->AddObject(camera);
	Driver::GetInstance()->BeginBehaviour(testScene);
}
