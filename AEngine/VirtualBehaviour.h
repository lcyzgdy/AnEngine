#pragma once
#ifndef __VIRTUALBEHAVIOUR_H__
#define __VIRTUALBEHAVIOUR_H__

#include"BaseBehaviour.h"
#include"onwind.h"
#include<mutex>

namespace AEngine::Game
{
	class VirtualBehaviour : public BaseBehaviour
	{
		friend class Scene;

	public:
		VirtualBehaviour() = default;
		VirtualBehaviour(const VirtualBehaviour& rhs) = default;
		~VirtualBehaviour() = default;
	};
}

#endif // !__VIRTUALBEHAVIOUR_H__
