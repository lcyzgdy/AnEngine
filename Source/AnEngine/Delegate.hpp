#pragma once
#ifndef __DELEGATE_H__
#define __DELEGATE_H__

#include "onwind.h"

namespace AnEngine
{
	class SameDelegateException : public std::exception
	{
	public:
		SameDelegateException() : std::exception("已经委托相同的函数")
		{
		}
	};

	namespace
	{
		template<class _C, typename _Ret, typename ..._Args>
		class _MemberFunctionWrapper
		{
			typedef _Ret(_C::* MemberFunctionPtrType)(_Args...);
			_C* m_classPtr;
			MemberFunctionPtrType m_func;

		public:
			_MemberFunctionWrapper(MemberFunctionPtrType fun, _C* class_ptr) :m_func(fun), m_classPtr(class_ptr) {}

			_Ret operator()(_Args&& ...args)
			{
				return (m_classPtr->*m_func)(args...);
			}


			bool operator==(const _MemberFunctionWrapper<_C, _Ret, _Args...>& other) const
			{
				if (m_classPtr == other.m_classPtr && m_func == other.m_func)
				{
					return true;
				}
				return false;
			}

			bool operator==(_MemberFunctionWrapper<_C, _Ret, _Args...>&& other) const
			{
				if (m_classPtr == other.m_classPtr && m_func == other.m_func)
				{
					return true;
				}
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
		~Delegate()
		{
			m_tasks.clear();
		}

		Delegate& operator+=(const _FunctionType& func)
		{
#if DEBUG || _DEBUG
			for (var it = m_tasks.begin(); it != m_tasks.end(); ++it)
			{
				var fPtr = (*it).target<_FunctionPrtType>();
				var gPtr = func.target<_FunctionPrtType>();
				if (fPtr && gPtr && *fPtr == *gPtr)
				{
					throw SameDelegateException();
				}
			}
#endif
			m_tasks.emplace_back(func);
			return *this;
		}

		Delegate& operator+=(_FunctionType&& func)
		{
#if _DEBUG
			for (var it = m_tasks.begin(); it != m_tasks.end(); ++it)
			{
				var fPtr = (*it).target<_FunctionPrtType>();
				var gPtr = func.target<_FunctionPrtType>();
				if (fPtr && gPtr && *fPtr == *gPtr)
				{
					throw SameDelegateException();
				}
			}
#endif
			m_tasks.emplace_back(func);
			return *this;
		}

		template<class _C>
		Delegate& operator+=(const _MemberFunctionWrapper<_C, _Rt, _Args...>& classMemFunc)
		{
#if _DEBUG
			var itr = std::find_if(m_tasks.begin(), m_tasks.end(),
				[&](_FunctionType& f)
				{
					var fPtr = f.target<_MemberFunctionWrapper<_C, _Rt, _Args...>>();
					if (fPtr && *fPtr == classMemFunc)
					{
						return true;
					}
					return false;
				});

			if (itr != m_tasks.end())
			{
				throw SameDelegateException();
			}
#endif
			m_tasks.emplace_back(classMemFunc);
			return *this;
		}

		template<class _C>
		Delegate& operator+=(_MemberFunctionWrapper<_C, _Rt, _Args...>&& classMemFunc)
		{
#if _DEBUG
			var itr = std::find_if(m_tasks.begin(), m_tasks.end(),
				[&](_FunctionType& f)
				{
					var fPtr = f.target<_MemberFunctionWrapper<_C, _Rt, _Args...>>();
					if (fPtr && *fPtr == classMemFunc)
					{
						return true;
					}
					return false;
				});

			if (itr != m_tasks.end())
			{
				throw SameDelegateException();
			}
#endif
			m_tasks.emplace_back(classMemFunc);
			return *this;
		}

		Delegate& operator-=(const _FunctionType& func)
		{
			for (var it = m_tasks.begin(); it != m_tasks.end(); ++it)
			{
				var fPtr = (*it).target<_FunctionPrtType>();
				var gPtr = func.target<_FunctionPrtType>();
				if (fPtr && gPtr && *fPtr == *gPtr)
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
				var fPtr = (*it).target<_FunctionPrtType>();
				var gPtr = func.target<_FunctionPrtType>();
				if (fPtr && gPtr && *fPtr == *gPtr)
				{
					m_tasks.erase(it);
					break;
				}
			}

			return *this;
		}

		template<class _C>
		Delegate& operator-=(const _MemberFunctionWrapper<_C, _Rt, _Args...>& classMemFunc)
		{
			var itr = std::find_if(m_tasks.begin(), m_tasks.end(),
				[&](_FunctionType& f)
				{
					var fPtr = f.target<_MemberFunctionWrapper<_C, _Rt, _Args...>>();
					if (fPtr && *fPtr == classMemFunc)
					{
						return true;
					}
					return false;
				});

			if (itr != m_tasks.end())
			{
				m_tasks.erase(itr);
			}
			return *this;
		}

		template<class _C>
		Delegate& operator-=(_MemberFunctionWrapper<_C, _Rt, _Args...>&& classMemFunc)
		{
			var itr = std::find_if(m_tasks.begin(), m_tasks.end(),
				[&](_FunctionType& f)
				{
					var _fun = f.target<_MemberFunctionWrapper<_C, _Rt, _Args...>>();
					if (_fun && *_fun == classMemFunc)
					{
						return true;
					}
					return false;
				});

			if (itr != m_tasks.end())
			{
				m_tasks.erase(itr);
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
		__forceinline auto MakeDelegateClassHelper(_Ret(_C::* class_func_ptr)(_Args...), _C* class_ptr)
		{
			return _MemberFunctionWrapper<_C, _Ret, _Args...>(class_func_ptr, class_ptr);
		}
	};
}

#endif // !__DELEGATE_H__