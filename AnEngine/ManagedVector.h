#pragma once
#ifndef __MANAGEDVECTOR_H__
#define __MANAGEDVECTOR_H__

#include <vector>
#include <memory>

namespace AnEngine::Utility
{
	template<typename T>
	class ManagedVector
	{
		std::vector<void*> m_ptr;
		size_t m_size;
	public:
		ManagedVector()
		{
			m_ptr.reserve(64);
			void* ptr = malloc(sizeof(T) * 256);
			m_ptr.push_back(prt);
		}
		~ManagedVector()
		{
			for (int i = 0; i < m_size; i++)
			{
				this->operator[](i).~T();
			}
			for (void* i : m_ptr) free(i);
		}

		void push_back(const T& rhs)
		{
			m_size++;
			if (m_size % 256 == 0)
			{
				m_ptr.push_back(malloc(sizeof(T) * 256));
			}
			void* p = this->operator[](m_size - 1);
			memcpy(p, &rhs, sizeof(T));
		}

		T& operator[](int index)
		{
			return *(m_ptr[index / 256] + (index % 256));
		}

		size_t size() { return m_size; }
	};
}

#endif // !__MANAGEDVECTOR_H__
