#pragma once
#include"Scene.h"
#include"Camera.h"
#include"ObjectBehaviour.h"

extern AEngine::Game::Scene* testScene;
extern AEngine::Game::Camera* testCamera;

void LoadScene();

class TestCamera :public AEngine::Game::ObjectBehaviour
{
};