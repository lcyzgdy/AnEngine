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

	template<typename T0, typename ...T>
	class BehaviourSystem
	{
		std::vector<GameEntity*> m_entities;
		const ScheduleMode m_how;

	public:
		BehaviourSystem(ScheduleMode scheduleMode) : m_entities(GameEntity::FindEntitiesByComponentType<T0, T...>()), m_how(scheduleMode)
		{
		}

		virtual ~BehaviourSystem() = default;
	};

	class IParallelSystem
	{
	public:
		virtual void Execute(int index) = 0;
	};
}

#endif // !__BEHAVIOURSYSTEM_H__
