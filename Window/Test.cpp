#include "Test.h"
#include"Driver.h"
using namespace AEngine;
using namespace AEngine::Game;

Scene* testScene;
Camera* testCamera;

void LoadScene()
{
	testScene = new Scene(L"Test Scene");
	testCamera = new Camera(L"Test Camera");
	testCamera->ClearFlag(Camera::ClearFlags::SolidColor);
	TestCamera* camera = new TestCamera(L"Test Camera Object");
	camera->AddComponent(testCamera);
	testScene->AddObject(camera);
	Driver::GetInstance()->BeginBehaviour(testScene);
}

TestCamera::TestCamera(std::wstring name) :ObjectBehaviour(name)
{
}
