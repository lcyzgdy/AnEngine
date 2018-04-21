#pragma once
#ifndef __HASH_HPP__
#define __HASH_HPP__

#include"onwind.h"
#include"DMath.hpp"

#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif // _M_X64

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif

namespace AnEngine::Utility
{
	inline uint64_t HashRange(const uint32_t* const begin, const uint32_t* const end, size_t hash)
	{
#if ENABLE_SSE_CRC32
		const uint64_t* iter64 = (const uint64_t*)DMath::AlignUp(begin, 8);
		const uint64_t* const end64 = (const uint64_t* const)DMath::AlignDown(end, 8);

		// 如果不是64位对齐则用32位
		if ((uint32_t*)iter64 > begin)
		{
			hash = _mm_crc32_u32((uint32_t)hash, *begin);
		}

		// 迭代64位值
		while (iter64 < end64)
		{
			hash = _mm_crc32_u64((uint64_t)hash, *iter64++);
		}

		// 如果有一个32位的余数，则累加
		if ((uint32_t*)iter64 < end)
			hash = _mm_crc32_u32((uint32_t)hash, *(uint32_t*)iter64);
#else
		// 如果CPU没有SSE4.2指令集，则使用散列
		for (const uint32_t* i = begin; i < end; ++i)
			hash = 16777619U * hash ^ *i;

#endif // ENABLE_SSE_CRC32
		return hash;
	}

	template<typename T>
	inline uint64_t GetHash(const T* stateDesc, size_t count = 1, uint64_t hash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "Object is not word-aligned");
		return HashRange((uint32_t*)stateDesc, (uint32_t*)(stateDesc + count), hash);
	}
}

#endif // !__HASH_HPP__

