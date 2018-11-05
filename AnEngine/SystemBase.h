#pragma once
#ifndef __SYSTEMBASE_H__
#define __SYSTEMBASE_H__

#include "Object.h"
#include "GameObject.h"
#include <bitset>

namespace AnEngine::Game::System
{
	class SystemBase : Object
	{
	protected:
		std::deque<GameObject*>& m_objectsInScene;

	public:
		explicit SystemBase(std::deque<GameObject*>& objInScene) : m_objectsInScene(objInScene) {}
		virtual ~SystemBase() = default;
	};
}

#endif // !__SYSTEMBASE_H__
