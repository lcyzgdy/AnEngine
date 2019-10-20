#include "ChunkAllocator.h"
using namespace std;

namespace AnEngine::Memory
{
	ChunkAllocator::ChunkAllocator()
	{
	}

	ChunkAllocator::~ChunkAllocator()
	{
		for (var chunk : m_chunksList)
		{
			delete chunk;
		}
		m_chunksList.clear();
	}

	Chunk* ChunkAllocator::Allocate()
	{
		var chunk = new Chunk();
		m_chunksList.push_back(chunk);
		return chunk;
	}

	void ChunkAllocator::Deallocate(Chunk* chunk)
	{
		var it = find(m_chunksList.begin(), m_chunksList.end(), chunk);
		if (it != m_chunksList.end())
		{
			m_chunksList.erase(it);
		}
		free(chunk);
	}
}