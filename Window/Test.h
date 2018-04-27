#pragma once
#include "Driver.h"

//extern AnEngine::Game::Scene* testScene;
//extern AnEngine::Game::Camera* testCamera;

void LoadScene();

class TestCamera : public AnEngine::Game::Script
{
protected:
	virtual void Update() override;
public:
	TestCamera();
};