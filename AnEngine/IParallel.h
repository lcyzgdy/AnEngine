#pragma once
#ifndef __BEHAVIOURSYSTEM_H__
#define __BEHAVIOURSYSTEM_H__

#include "GameEntity.h"
#include <tuple>

namespace AnEngine::Game
{
	enum ScheduleMode
	{
		Sequence,
		Parallel,
		GroupBatch
	};

	class IParallel
	{
	public:
		virtual void Execute(int index) = 0;
		int Length;
	};
}

#endif // !__BEHAVIOURSYSTEM_H__
