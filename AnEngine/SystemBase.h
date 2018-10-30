#pragma once
#ifndef __SYSTEMBASE_H__
#define __SYSTEMBASE_H__

#include "Object.h"
#include "GameObject.h"
#include <bitset>

namespace AnEngine::Game::System
{
	template<size_t N = 256>
	class SystemBase : Object
	{
		std::bitset<N> m_activeObj;
		std::vector<GameObject>& m_objectsInScene;

	public:
		explicit SystemBase(std::vector<GameObject>& objInScene) : m_objectsInScene(objInScene) {}
		virtual ~SystemBase() = default;
	};
}

#endif // !__SYSTEMBASE_H__
