#include "Archetype.h"
using namespace std;

namespace AnEngine
{
	Archetype::Archetype(std::initializer_list<size_t>&& init_list) :m_typeids(init_list)
	{
	}
	Archetype::Archetype(const std::vector<size_t>& right_v) : m_typeids(right_v)
	{
	}
	Archetype::Archetype(std::vector<size_t>&& right_v) : m_typeids(right_v)
	{
	}
	bool Archetype::operator==(const Archetype& other)
	{
		if (other.m_typeids.size() != m_typeids.size())
		{
			return false;
		}
		for (int i = 0; i < m_typeids.size(); i++)
		{
			if (other.m_typeids[i] != m_typeids[i])
			{
				return false;
			}
		}
		return true;
	}
	bool Archetype::operator==(Archetype&& other)
	{
		if (other.m_typeids.size() != m_typeids.size())
		{
			return false;
		}
		for (int i = 0; i < m_typeids.size(); i++)
		{
			if (other.m_typeids[i] != m_typeids[i])
			{
				return false;
			}
		}
		return true;
	}

	void Archetype::Combine(const Archetype& other)
	{
		m_typeids.insert(m_typeids.end(), other.m_typeids.begin(), other.m_typeids.end());
	}

	void Archetype::Combine(Archetype&& other)
	{
		m_typeids.insert(m_typeids.end(), other.m_typeids.begin(), other.m_typeids.end());
	}
}