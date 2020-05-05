#pragma once
#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <string>
#include "onwind.h"
#include "Hash.hpp"
#include <sstream>

namespace AnEngine
{
	class DLL_API Object
	{
	public:
		virtual ~Object() = default;
		inline virtual uint64_t GetHashCode() { return Utility::GetHash(this); }
		inline virtual std::string ToString() { return "Object"; }

		template<typename T>
		bool ReferenceEqual(T* rhs)
		{
			return this == rhs;
		}
	};
}

#endif // !__OBJECT_H__
