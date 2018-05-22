#pragma once
#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <string>

namespace AnEngine
{
	class Object
	{
	public:
		virtual ~Object();
		virtual uint64_t GetHashCode();
		virtual std::wstring ToString();
		template<typename T>
		bool ReferenceEqual(T* rhs)
		{
			return this == rhs;
		}
	};
}

#endif // !__OBJECT_H__
