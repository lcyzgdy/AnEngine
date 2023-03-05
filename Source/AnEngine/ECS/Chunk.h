#pragma once
#ifndef __CHUNK_H__
#define __CHUNK_H__

#include "onwind.h"

namespace AnEngine::Memory
{
	struct Chunk
	{
	public:
		uint32_t entitiesCount;
		std::byte* memory;		// Chunk块开头
		std::size_t offset;
		std::size_t totalSize;
		std::size_t entitySize;

		Chunk();
		~Chunk();

		std::size_t FreeSize();
	};
}

#endif // !__CHUNK_H__
