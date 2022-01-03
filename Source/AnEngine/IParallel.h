#pragma once
#ifndef __IPARALLER_H__
#define __IPARALLER_H__

#include <tuple>
#include <type_traits>

#include "Component.h"

namespace AnEngine::Game
{
	enum ScheduleMode
	{
		Sequence,
		Parallel,
		GroupBatch
	};

	template<typename ..._ComponentType>
	class IParallel;

	/*
	 * Job 是纯执行方法，可依赖 Component 中的数据，不可依赖 Behaviour 的数据。
	 * Job 有两种，这里是可并行执行的 Job，因此不建议此处依赖其他 Entity 的Component，否则性能会大幅下降，而且可能死锁。
	 */
	template<>
	class IParallel<>
	{
	public:
		virtual void Execute(int index) = 0;
		virtual bool Check(int index) { return true; }
		size_t Length;
		// virtual ~IParallel() = 0;
	};

	template<typename ..._ComponentType>
	class IParallel : public IParallel<>
	{
	};
}

#endif // !__IPARALLER_H__
