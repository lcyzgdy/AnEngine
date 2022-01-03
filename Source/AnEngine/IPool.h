#pragma once
#ifndef __IPOOL_H__
#define __IPOLL_H__

#include <tuple>

namespace AnEngine
{
	template<typename ... _T>
	class IPool
	{
	public:
		virtual std::tuple<_T...>&& GetOne() = 0;
		virtual void Push(_T...) = 0;
	};
}

#endif // !__IPOOL_H__