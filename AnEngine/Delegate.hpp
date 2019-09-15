#pragma once
#ifndef __DELEGATE_H__
#define __DELEGATE_H__

#include "onwind.h"

namespace AnEngine
{
	template<typename Rt, typename ... Args>
	class DLL_API Delegate
	{
		std::deque<std::function<Rt(Args)>> m_tasks;

	public:
		Delegate& operator+=(const std::function<Rt(Args...)>& func)
		{
#ifdef _DEBUG
			for (var i : m_tasks)
			{
				if (i.target() == func.target()) return *this;
			}
#endif
			m_tasks.push_back(func);
			return *this;
		}

		Delegate& operator+=(std::function<Rt(Args...)>&& func)
		{
#ifdef _DEBUG
			for (var i : m_tasks)
			{
				if (i.target() == func.target()) return *this;
			}
#endif
			m_tasks.emplace_back(func);
			return *this;
		}

		Delegate& operator-=(const std::function<Rt(Args...)>& func)
		{
			for (std::deque<std::function<Rt(Args...)>>::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
			{
				if (func.target() == (*it).target())
				{
					m_tasks.erase(it);
					break;
				}
			}
			return *this;
		}
	};
}

#endif // !__DELEGATE_H__