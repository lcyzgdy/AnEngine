#pragma once
#ifndef __BASEBEHAVIOUR_H__
#define __BASEBEHAVIOUR_H__

#include<mutex>

namespace AnEngine
{
	class Driver;
}

namespace AnEngine::Game
{
	class BaseBehaviour
	{
		friend class ::AnEngine::Driver;
		friend class Scene;
		// virtual void OnRender() = 0;
		// virtual void OnUpdate() = 0;

		virtual void OnInit() = 0;
		//virtual void BeforeUpdate() = 0;
		virtual void OnUpdate() = 0;
		//virtual void AfterUpdate() = 0;
		virtual void OnRelease() = 0;

	protected:
		std::recursive_mutex m_recursiveMutex;
		//bool m_enable;

	public:
		BaseBehaviour() = default;
		BaseBehaviour(const BaseBehaviour& rhs) = default;
		~BaseBehaviour() = default;
	};
}

#endif // !__BASEBEHAVIOUR_H__
