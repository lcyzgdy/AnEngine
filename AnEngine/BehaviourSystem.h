#pragma once
#ifndef __BEHAVIOURSYSTEM_H__
#define __BEHAVIOURSYSTEM_H__

#include "GameObject.h"
#include <tuple>

namespace AnEngine::Game
{
	enum ScheduleMode
	{
		Sequence,
		Parallel,
		GroupBatch
	};

	template<typename ...T>
	class BehaviourSystem
	{
		void SystemInitialize()
		{
			std::list<GameObject*> entities;
		}
	public:
		virtual ~BehaviourSystem() = default;
	};
}

#endif // !__BEHAVIOURSYSTEM_H__
