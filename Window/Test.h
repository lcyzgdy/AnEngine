#pragma once
#ifndef __TEST_H__
#define __TEST_H__

#include"Scene.h"
#include"Camera.h"

extern Scene* scene;
extern Camera* defaultCamera;

void CreateScene();
void CreateCamera();

#endif // !__TEST_H__
