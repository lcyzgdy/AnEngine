#pragma once
#ifndef __DELEGATE_H__
#define __DELEGATE_H__

#include "onwind.h"

namespace AnEngine
{
	namespace
	{
		template<class _C, typename _Rt, typename... _Args>
		class MemberFunctionWrapper
		{

		};
	}

	template<typename _Rt, typename ... _Args>
	class DLL_API Delegate
	{
		using _FunctionType = std::function<_Rt(_Args...)>;
		using _FunctionPrtType = _Rt(*)(_Args...);

		std::list<_FunctionType> m_tasks;

	public:
		Delegate& operator+=(const _FunctionType& func)
		{
			for (var i : m_tasks)
			{
				if (*(i.target<_FunctionPrtType>()) == *(func.target<_FunctionPrtType>())) return *this;
			}
			m_tasks.push_back(func);
			return *this;
		}

		Delegate& operator+=(_FunctionType&& func)
		{
			for (var i : m_tasks)
			{
				if (*(i.target<_FunctionPrtType>()) == *(func.target<_FunctionPrtType>())) return *this;
			}
			m_tasks.emplace_back(func);
			return *this;
		}

		Delegate& operator-=(const _FunctionType& func)
		{
			for (var it = m_tasks.begin(); it != m_tasks.end(); ++it)
			{
				if (*(func.target<_FunctionPrtType>()) == *((*it).target<_FunctionPrtType>()))
				{
					m_tasks.erase(it);
					break;
				}
			}
			return *this;
		}

		Delegate& operator-=(_FunctionType&& func)
		{
			for (var it = m_tasks.begin(); it != m_tasks.end(); ++it)
			{
				if (*(func.target<_FunctionPrtType>()) == *((*it).target<_FunctionPrtType>()))
				{
					m_tasks.erase(it);
					break;
				}
			}
			return *this;
		}

		auto operator()(_Args... args)
		{
			if constexpr (std::is_same<_Rt, void>::value)
			{
				for (var l : m_tasks)
				{
					l(args...);
				}
			}
			else
			{
				std::vector<_Rt> res;
				for (var l : m_tasks)
				{
					res.emplace_back(l(args...));
				}
				return res;
			}
		}
	};
}

#endif // !__DELEGATE_H__