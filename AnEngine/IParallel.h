#pragma once
#ifndef __IPARALLER_H__
#define __IPARALLER_H__

//#include "GameEntity.h"
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
		virtual bool Check(int index) { return true; }
		size_t Length;
	};
}

#endif // !__IPARALLER_H__
