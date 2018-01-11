#include "Test.h"
#include"Driver.h"
using namespace AnEngine;
using namespace AnEngine::Game;

Scene* testScene;
Camera* testCamera;

void LoadScene()
{
	testScene = new Scene(L"Test Scene");
	testCamera = new Camera(L"Test Camera");
	testCamera->ClearFlag(Camera::ClearFlags::SolidColor);
	testCamera->ClearColor(Color::Blue);
	TestCamera* camera = new TestCamera(L"Test Camera Object");
	camera->AddComponent(testCamera);
	testScene->AddObject(camera);
	Driver::GetInstance()->BeginBehaviour(testScene);
}

TestCamera::TestCamera(std::wstring name) :ObjectBehaviour(name)
{
}
