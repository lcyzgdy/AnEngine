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
	testCamera = new Camera(L"Test Camera");
	testCamera->ClearFlag(Camera::ClearFlags::SolidColor);
	testCamera->ClearColor(Color::Blue);

	TestCamera* camera = new TestCamera(L"Test Object");
	camera->AddComponent(testCamera);

	TrangleRender* trangleRender = new TrangleRender(L"Test Render");
	camera->AddComponent(trangleRender);
	//ParticlesRenderer* nBody = new ParticlesRenderer(L"Test Particles");
	//camera->AddComponent(nBody);

	testScene->AddObject(camera);
	Driver::GetInstance()->StartScene(testScene);
}

void TestCamera::Update()
{
	var camera = GetComponent<Camera>();
	camera->ClearColor({ 0.0f, sin((float)Timer::GetTotalTicks() / 180000), sin((float)Timer::GetTotalTicks() / 300000), 1.0f });
}

TestCamera::TestCamera(std::wstring&& name) : ObjectBehaviour(name)
{
}
