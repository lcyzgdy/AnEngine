#pragma once
#ifndef __TRANSFORMSYSTEM_H__
#define __TRANSFORMSYSTEM_H__

#include "BehaviourSystem.h"
#include "Transform.h"

namespace AnEngine::Game
{
	struct DataGroup
	{
		std::vector<Transform*> transform;

		Transform* operator[](int i)
		{
			return transform[i];
		}
	};

	class TransformSystem : public BehaviourSystem<Transform>, public IParallelSystem
	{
		DataGroup m_data;
	public:
		// 通过 IParallelSystem 继承
		virtual void Execute(int index) override;
	};
}

#endif // !__TRANSFORMSYSTEM_H__
