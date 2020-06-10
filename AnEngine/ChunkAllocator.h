#pragma once
#ifndef __CHUNKALLOCATOR_H__
#define __CHUNKALLOCATOR_H__

#include "onwind.h"
#include "Chunk.h"
#include "AllocatorBase.h"
#include "Singleton.h"

namespace AnEngine::Memory
{
	class DLL_API ChunkAllocator : public Utility::Singleton<ChunkAllocator>, public AllocatorBase
	{
		friend class Utility::Singleton<ChunkAllocator>;

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
