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

		std::deque<std::shared_ptr<Chunk>> m_chunksList;

	protected:
		ChunkAllocator();
		~ChunkAllocator();

	public:
		std::shared_ptr<Chunk> Allocate();
		void Deallocate(Chunk* chunk);
	};
}

#endif // !__CHUNKMANAGER_H__
