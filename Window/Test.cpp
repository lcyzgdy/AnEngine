#include "Test.h"
#include "onwind.h"
#include "SampleMeshRender.h"
#include "ParticlesRenderer.h"
#include "DTimer.h"
#include "StateMachine.h"
#include "DebugLog.h"
#include "Canvas.h"
#include "UIText.h"
#include "Input.h"
#include "Camera.h"
using namespace AnEngine;
using namespace AnEngine::Game;
using namespace AnEngine::RenderCore;
using namespace AnEngine::Game::UI;

Scene* testScene;
Camera* testCamera;

void LoadScene()
{
	/*testScene = new Scene(L"Test Scene");
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
	int s1 = fsm->CreateNewState(L"State1", [] {});
	int s2 = fsm->CreateNewState(L"State2", [] {});
	int s3 = fsm->CreateNewState(L"State3", [] {});
	fsm->AddFloatParam(L"Sin", 0f);
	fsm->AddTrigerParam(L"Trigger");
	fsm->CreateStateTransCondition(s1, s2, L"Sin", 0f, StateMachine::Condition::LessOrEqual);
	fsm->CreateStateTransCondition(s2, s1, L"Sin", 0f, StateMachine::Condition::Greater);
	fsm->CreateStateTransCondition(s1, s3, L"T");
	fsm->CreateStateTransCondition(s3, s1, L"Sin", 0.5f, StateMachine::Condition::Less);
	testCameraObject->AddComponent(fsm);///

	GameObject* canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();
	GameObject* debugText = new GameObject(L"Text");
	debugText->AddComponent<UIText>();
	debugText->GetComponent<UIText>()->Text(L"Test");
	canvas->AddChildObject(debugText);

	testScene->AddObject(testCameraObject);
	testScene->AddObject(canvas);
	Engine::Instance()->StartScene();*/
}

void TestCamera::Update()
{
	//var cameraScript = gameObject->GetComponent<Camera>();
	//cameraScript->ClearColor({ sin((float)Timer::GetTotalTicks() / 240000), sin((float)Timer::GetTotalTicks() / 180000), sin((float)Timer::GetTotalTicks() / 300000), 1.0f });
	//var stateMachine = gameObject->GetComponent<StateMachine>();
	/*if (BaseInput::GetInstance()->GetKeyDown(48))
	{
		stateMachine->SetTrigger(L"T");
	}*/
	//stateMachine->SetFloat(L"Sin", sin((float)Timer::GetTotalTicks() / 240000));
	//GameObject::Find(L"Text")->GetComponent<UIText>()->Text(stateMachine->GetCurrectStateName());
	//GameObject::Find(L"Text")->GetComponent<UIText>()->Text(to_wstring(Timer::GetFPS()));
}

TestCamera::TestCamera() : Script()
{
}
