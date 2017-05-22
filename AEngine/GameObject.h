#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include"onwind.h"

class GameObject
{
public:
	GameObject() = default;
	~GameObject() = default;

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();
};

#endif // !__GAMEOBJECT_H__
