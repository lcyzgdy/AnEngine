#pragma once
#ifndef __CHUNKALLOCATOR_H__
#define __CHUNKALLOCATOR_H__

#include "onwind.h"
#include "Chunk.h"
#include "AllocatorBase.h"

namespace AnEngine::Memory
{
	class ChunkAllocator : public Singleton<ChunkAllocator>, public AllocatorBase
	{
		friend class Singleton<ChunkAllocator>;

		std::list<Chunk*> m_chunks;

	private:
		ChunkAllocator();
		~ChunkAllocator();

	public:
		Chunk* Allocate();
	};
}

#endif // !__CHUNKMANAGER_H__
