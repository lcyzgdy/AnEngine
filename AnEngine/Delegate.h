#pragma once
#ifndef __DELEGATE_HPP__
#define __DELEGATE_HPP__

#include "onwind.h"

namespace AnEngine
{
	template<typename Rt, typename ... Args>
	class DLL_API Delegate
	{
		std::deque<std::function<Rt<Args...>>> m_delegatedFuncs;

	public:
		Delegate& operator+=(const std::function<Rt(Args...)>& func)
		{
		}

		Delegate& operator+=(std::function<Rt(Args...)>&& func)
		{
		}
	};
}

#endif // !__DELEGATE_HPP__
