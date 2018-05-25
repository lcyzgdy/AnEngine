#pragma once
#ifndef __MANAGEDTASK_HPP__
#define __MANAGEDTASK_HPP__

#include<functional>
#include<queue>
#include"Object.h"

namespace AnEngine::Utility
{
	template<typename _RtTy, typename... _Ty>
	class ManagedTask : public Object
	{
		std::function<_RtTy(_Ty...)> m_task;
	public:
		_RtTy Invoke(_Ty... args)
		{
			return m_task(args...);
		}
	};

	template<typename _RtTy>
	class ManagedTask : public Object
	{
		std::function<_RtTy()> m_task;
	public:
		_RtTy Invoke()
		{
			return m_task();
		}
	};
}

#endif // !__MANAGEDTASK_HPP__
