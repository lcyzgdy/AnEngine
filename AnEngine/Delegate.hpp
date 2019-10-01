#pragma once
#ifndef __DELEGATE_H__
#define __DELEGATE_H__

#include "onwind.h"

namespace AnEngine
{
	namespace
	{
		template<class _C, typename _Ret, typename ..._Args>
		class MemberFunctionWrapper
		{
			typedef _Ret(_C::* MemberFunctionPtrType)(_Args...);
			_C* m_classPtr;
			MemberFunctionPtrType m_func;

		public:
			MemberFunctionWrapper(MemberFunctionPtrType fun, _C* class_ptr) :m_func(fun), m_classPtr(class_ptr) {}

			_Ret operator()(_Args&& ...args)
			{
				return (m_classPtr->*m_func)(args...);
			}


			bool operator==(const MemberFunctionWrapper<_C, _Ret, _Args...>& other) const
			{
				if (m_classPtr == other.m_classPtr && m_func == other.m_func)
					return true;
				return false;
			}

			bool operator==(MemberFunctionWrapper<_C, _Ret, _Args...>&& other) const
			{
				if (m_classPtr == other.m_classPtr && m_func == other.m_func)
					return true;
				return false;
			}

			template<typename T>
			bool operator==(const T&&)const
			{
				return false;
			}
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

		template<class _C>
		Delegate& operator+=(MemberFunctionWrapper<_C, _Rt, _Args...>&& classMemFunc)
		{
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

	namespace Utility
	{
		template<typename _C, typename _Ret, typename ..._Args>
		auto MakeDelegateClassHelper(_Ret(_C::* class_func_type)(_Args...), _C* class_ptr)
		{
			return MemberFunctionWrapper<_C, _Ret, _Args...>(class_func_type, class_ptr);
		}
	};
}

#endif // !__DELEGATE_H__