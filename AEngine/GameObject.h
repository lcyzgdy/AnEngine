#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include"onwind.h"
#include"PhysicsCore.h"
using namespace PhysicsCore;

class GameObject
{
	vector<GameObject*> content;
	Transform transform;

public:
	GameObject();
	~GameObject() = default;

	string name;
	GameObject* const gameObject;
	GameObject* parentObject;
	vector<GameObject*> childrenObject;

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

	template<typename T>
	T* GetContent(string& name)
	{
		for (auto i = 0; i < content.size(); i++)
		{
			if (content[i].name == name) return static_cast<T*>(gameObject);
		}
		return nullptr;
	}
};

#endif // !__GAMEOBJECT_H__
