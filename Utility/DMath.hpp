#pragma once
#ifndef __DMATH_HPP__
#define __DMATH_HPP__

namespace DMath
{
	template<typename T>
	__forceinline T AlignUpWithMask(T value, size_t mask)
	{
		return (T)(((size_t)value + mask) & ~mask);
	}

	template<typename T>
	__forceinline T AlignDownWithMask(T value, size_t mask)
	{
		return (T)((size_t)value & ~mask);
	}

	template<typename T>
	__forceinline T AlignUp(T value, size_t alignment)
	{
		return AlignUpWithMask(value, alignment - 1);
	}

	template<typename T>
	__forceinline T AlignDown(T value, size_t alignment)
	{
		return AlignDownWithMask(value, alignment - 1);
	}

	template<typename T>
	__forceinline bool IsAligned(T value, size_t alignment)
	{
		return 0 == ((size_t)value & (alignment - 1));
	}

	template<typename T>
	__forceinline T& Min(T& t ...)
	{
		size_t size = sizeof(T);
		T* args = &t;
		T* minn = args;
		args += size;
		while ((*args) != static_cast<T>(0))
		{
			minn = ((*args) < (*minn)) ? args : minn;
			args += size;
		}
		return *minn;
	}

	template<typename T>
	__forceinline T& Max(T& t ...)
	{
		size_t size = sizeof(T);
		T* args = &t;
		T* minn = args;
		args += size;
		while ((*args) != static_cast<T>(0))
		{
			minn = ((*args) < (*minn)) ? minn : args;
			args += size;
		}
		return *minn;
	}
}

#endif // !__DMATH_HPP__
