#include "Chunk.h"
#include <cstdlib>
using namespace std;

namespace AnEngine::Memory
{
	Chunk::Chunk() :entitiesCount(0), offset(0), totalSize(sizeof(std::byte) * 16 * 1024), entitySize(0),
		memory(static_cast<std::byte*>(malloc(sizeof(std::byte) * 16 * 1024)))	// 分配16KB空间
	{

	}

	Chunk::~Chunk()
	{
		free(static_cast<void*>(memory));
	}

	size_t Chunk::FreeSize()
	{
		return totalSize - entitySize * entitiesCount;
	}
}
