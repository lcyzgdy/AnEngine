#include "ChunkAllocator.h"
using namespace std;

namespace AnEngine::Memory
{
	ChunkAllocator::ChunkAllocator()
	{
	}

	ChunkAllocator::~ChunkAllocator()
	{
		/*for (var chunk : m_chunksList)
		{
			// delete chunk;
			chunk.
		}*/
		m_chunksList.clear();
	}

	shared_ptr<Chunk> ChunkAllocator::Allocate()
	{
		var chunk = make_shared<Chunk>();
		m_chunksList.emplace_back(chunk);
		return chunk;
	}

	void ChunkAllocator::Deallocate(Chunk* chunk)
	{
		/*var it = find(m_chunksList.begin(), m_chunksList.end(), chunk);
		if (it != m_chunksList.end())
		{
			m_chunksList.erase(it);
		}
		free(chunk);*/
	}
}