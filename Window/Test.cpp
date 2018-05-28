#include "Test.h"
#include "onwind.h"
#include "SampleMeshRender.h"
#include "ParticlesRenderer.h"
#include "DTimer.h"
#include "StateMachine.h"
#include "DebugLog.h"
#include "Canvas.h"
#include "UIText.h"
using namespace AnEngine;
using namespace AnEngine::Game;
using namespace AnEngine::RenderCore;
using namespace AnEngine::Game::UI;

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

	/*StateMachine* fsm = new StateMachine();
	int s1 = fsm->CreateNewState(L"State1", []() { Debug::Log(L"State1"); });
	int s2 = fsm->CreateNewState(L"State2", []() { Debug::Log(L"State2"); });
	fsm->AddFloatParam(L"Sin", 0f);
	fsm->CreateStateTransCondition(s1, s2, L"Sin", 0f, StateMachine::Condition::LessOrEqual);
	fsm->CreateStateTransCondition(s2, s1, L"Sin", 0f, StateMachine::Condition::Greater);
	testCameraObject->AddComponent(fsm);*/

	GameObject* canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();
	GameObject* debugText = new GameObject(L"Text");
	debugText->AddComponent<UIText>();
	debugText->GetComponent<UIText>()->Text(L"Test");
	canvas->AddChildObject(debugText);

	testScene->AddObject(testCameraObject);
	testScene->AddObject(canvas);
	Driver::GetInstance()->StartScene(testScene);
}

void TestCamera::Update()
{
	//var cameraScript = gameObject->GetComponent<Camera>();
	//cameraScript->ClearColor({ sin((float)Timer::GetTotalTicks() / 240000), sin((float)Timer::GetTotalTicks() / 180000), sin((float)Timer::GetTotalTicks() / 300000), 1.0f });
	//gameObject->GetComponent<StateMachine>()->SetFloat(L"Sin", sin((float)Timer::GetTotalTicks() / 240000));
}

TestCamera::TestCamera() : Script()
{
}
