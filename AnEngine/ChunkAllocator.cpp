#include "ChunkAllocator.h"

namespace AnEngine::Memory
{
	ChunkAllocator::ChunkAllocator()
	{
	}

	ChunkAllocator::~ChunkAllocator()
	{
		for (var chunk : m_chunks)
		{
			delete chunk;
		}
		m_chunks.clear();
	}
	Chunk* ChunkAllocator::Allocate()
	{
		var chunk = new Chunk();
		m_chunks.push_back(chunk);
		return chunk;
	}
}