﻿#pragma once
#ifndef __BASEBEHAVIOUR_H__
#define __BASEBEHAVIOUR_H__

#include <mutex>
#include "onwind.h"
#include "Object.h"

namespace AnEngine
{
	class Engine;
}

namespace AnEngine::Game
{
	class BaseBehaviour : public NonCopyable, public Object
	{
		friend class Engine;

		enum BehaviourState : int
		{
			BeforeFrameUpdate,
			FrameUpdate,
			AfterFrameUpdate
		};

		virtual void OnInit() = 0;
		virtual void BeforeUpdate() = 0;
		virtual void OnUpdate() = 0;
		virtual void AfterUpdate() = 0;
		virtual void OnRelease() = 0;

	protected:
		std::mutex m_mutex;
		bool m_released;

	public:
		BaseBehaviour() = default;
		virtual ~BaseBehaviour() = default;
	};
}

#endif // !__BASEBEHAVIOUR_H__
