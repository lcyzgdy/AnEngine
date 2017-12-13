#pragma once
#ifndef __BASEBEHAVIOUR_H__
#define __BASEBEHAVIOUR_H__

namespace AEngine
{
	class Driver;
}

namespace AEngine::Game
{
	class BaseBehaviour
	{
		friend class ::AEngine::Driver;
		friend class Scene;
		// virtual void OnRender() = 0;
		// virtual void OnUpdate() = 0;

	//protected:
		virtual void OnInit() = 0;
		virtual void OnRunning() = 0;
		virtual void OnRelease() = 0;

	public:
		BaseBehaviour() = default;
		BaseBehaviour(const BaseBehaviour& rhs) = default;
		~BaseBehaviour() = default;
	};
}

#endif // !__BASEBEHAVIOUR_H__
