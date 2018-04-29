#include "Test.h"
#include "onwind.h"
#include "SampleMeshRender.h"
#include "ParticlesRenderer.h"
#include "DTimer.h"
using namespace AnEngine;
using namespace AnEngine::Game;
using namespace AnEngine::RenderCore;

Scene* testScene;
Camera* testCamera;

void LoadScene()
{
	testScene = new Scene(L"Test Scene");
	GameObject* testCameraObject = new GameObject(L"Test Object");

	testCamera = new Camera();
	testCamera->ClearFlag(Camera::ClearFlags::SolidColor);
	testCamera->ClearColor({ 0.5f, 0.5f, 0.2f, 1.0f });
	testCameraObject->AddComponent(testCamera);

	TestCamera* cameraScript = new TestCamera();
	testCameraObject->AddComponent(cameraScript);

	TrangleRender* trangleRender = new TrangleRender();
	testCameraObject->AddComponent(trangleRender);
	//ParticlesRenderer* nBody = new ParticlesRenderer();
	//testCameraObject->AddComponent(nBody);

	testScene->AddObject(testCameraObject);
	Driver::GetInstance()->StartScene(testScene);
}

void TestCamera::Update()
{
	//var cameraScript = gameObject->GetComponent<Camera>();
	//cameraScript->ClearColor({ 0.0f, sin((float)Timer::GetTotalTicks() / 180000), sin((float)Timer::GetTotalTicks() / 300000), 1.0f });
}

TestCamera::TestCamera() : Script()
{
}
