#pragma once
#ifndef __CHUNK_H__
#define __CHUNK_H__

#include "onwind.h"

namespace AnEngine::Memory
{
	struct Chunk
	{
		uint32_t m_entitiesCount;
		std::byte* m_memBegin;		// Chunk块开头
	public:
		Chunk();
		~Chunk();
	};
}

#endif // !__CHUNK_H__
