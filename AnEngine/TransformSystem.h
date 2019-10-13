#pragma once
#ifndef __TRANSFORMSYSTEM_H__
#define __TRANSFORMSYSTEM_H__

#include "IParallel.h"

namespace AnEngine::Game
{
	class TransformSystem : public IParallel
	{
	public:
		// 通过 IParallel 继承
		virtual void Execute(int index) override;
	};
}

#endif // !__TRANSFORMSYSTEM_H__
