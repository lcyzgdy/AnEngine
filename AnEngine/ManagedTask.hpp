#pragma once
#ifndef __MANAGEDTASK_HPP__
#define __MANAGEDTASK_HPP__

#include<functional>
#include <queue>
#include "Object.h"
#include <tuple>

namespace AnEngine::Utility
{
	template<typename _F, typename... _Ty>
	class ManagedTask
	{
		using FuncType = std::_Binder<std::_Unforced, _F, _Ty...>;
		FuncType m_task;

	public:
		ManagedTask(_F&& f, _Ty&&... args) : m_task(std::bind(std::forward<_F>(f), std::forward<_Ty>(args)...))
		{
		}
		ManagedTask(ManagedTask<_F, _Ty...>&&) = delete;
		ManagedTask(const ManagedTask<_F, _Ty...>&) = delete;

		auto Invoke()
		{
			return m_task();
		}
	};
}

#endif // !__MANAGEDTASK_HPP__
