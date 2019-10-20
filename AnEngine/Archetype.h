#pragma once
#ifndef __ARCHETYPE_H__
#define __ARCHETYPE_H__

#include "onwind.h"
#include "Chunk.h"

namespace AnEngine
{
	struct Archetype
	{
	private:
		std::vector<size_t> m_typeids;
		std::list<Memory::Chunk*> m_chunks;
	public:
		Archetype(std::initializer_list<size_t>&& init_list);
		Archetype(const std::vector<size_t>& right_v);
		Archetype(std::vector<size_t>&& right_v);

		bool operator==(const Archetype& other);
		bool operator==(Archetype&& other);

		void Combine(const Archetype& other);
		void Combine(Archetype&& other);

		__forceinline void Sort() { std::sort(m_typeids.begin(), m_typeids.end()); std::unique(m_typeids.begin(), m_typeids.end()); }
	};
}

#endif // !__ARCHETYPE_H__
