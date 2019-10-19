#include "Chunk.h"
#include <cstdlib>
using namespace std;

namespace AnEngine::Memory
{
	Chunk::Chunk() :m_entitiesCount(0),
		m_memBegin(static_cast<std::byte*>(malloc(sizeof(std::byte) * 16 * 1024)))	// 分配16KB空间
	{

	}
	Chunk::~Chunk()
	{
		free(static_cast<void*>(m_memBegin));
	}
}
