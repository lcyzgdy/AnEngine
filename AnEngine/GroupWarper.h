#pragma once
#ifndef __GROUPWARPER_H__
#define __GROUPWARPER_H__

#include "onwind.h"
#include "ComponentGroup.hpp"

using namespace std;

namespace AnEngine::Game
{
	class GroupWarper
	{
		void* m_warper;
		uint64_t m_size;
		uint64_t m_typeHash;

	public:
		explicit GroupWarper(uint64_t hash, void* group, size_t size) : m_typeHash(hash), m_warper(group), m_size(size)
		{ 		}

		template<typename T>
		ComponentGroup<T>* GetGroup()
		{
			if (typeid(T).hash_code() != m_typeHash) throw exception("类型不对");
			return (ComponentGroup<T>*)m_warper;
		}
	};
}

#endif // !__GROUPWARPER_H__
