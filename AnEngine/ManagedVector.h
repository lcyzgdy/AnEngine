#pragma once
#ifndef __MANAGEDVECTOR_H__
#define __MANAGEDVECTOR_H__

#include <vector>
#include <memory>
#include <set>
#include <cstring>
#include <functional>

namespace AnEngine::Utility
{
	template<typename T>
	class ManagedVector
	{
		size_t m_size;
		std::vector<T*> m_ptr;
		std::set<uint32_t> m_freePos;
		T* m_curBlock;

	public:
		ManagedVector() : m_size(0)
		{
			m_ptr.reserve(64);
			m_curBlock = (T*)malloc(sizeof(T) * 256);
			m_ptr.push_back(m_curBlock);
		}
		~ManagedVector()
		{
			if (std::is_class<T>::value)
			{
				for (int i = 0; i < m_size; i++)
				{
					//cout << &((*this)[i]) << ' ';
					((*this)[i]).~T();
				}
			}
			for (auto i : m_ptr) free(i);
			/*for (auto i : m_ptr)
			{
				cout << i << ' ';
				free(i);
			}*/
		}

		T& operator[](int index)
		{
			return *(m_ptr[index / 256] + index % 256);
		}

		void push_back(const T& t)
		{
			if (!m_freePos.empty())
			{
				size_t index = m_freePos.begin();
				m_freePos.erase(m_freePos.begin());
				T* p = m_ptr[index / 256] + index % 256;
				memcpy(p, &t, sizeof(T));
				return;
			}
			if (m_size != 0 && m_size % 256 == 0)
			{
				m_curBlock = (T*)malloc(sizeof(T) * 256);
				m_ptr.push_back(m_curBlock);
			}
			T* p = m_curBlock + m_size % 256;
			memcpy(p, &t, sizeof(T));
			m_size++;
		}

		size_t size() { return m_size; }

		void Remove(uint32_t index)
		{
			if (!Exist(index)) throw std::exception("T has not existed");
			m_freePos.emplace(index);
			if (std::is_class<T>::value)
			{
				(m_ptr[index / 256] + index % 256).~T();
			}
		}

		T* Place()
		{
			if (!m_freePos.empty())
			{
				size_t index = m_freePos.begin();
				m_freePos.erase(m_freePos.begin());
				T* p = m_ptr[index / 256] + index % 256;
				return p;
			}
			if (m_size != 0 && m_size % 256 == 0)
			{
				m_curBlock = (T*)malloc(sizeof(T) * 256);
				m_ptr.push_back(m_curBlock);
			}
			return m_curBlock + m_size % 256;
		}

		bool Exist(uint32_t index)
		{
			return m_freePos.find(index) != m_freePos.end();
		}

		uint32_t Find(std::function<bool(const T&, const T&)> f, const T& val)
		{
			for (int i = 0; i < m_size; i++)
			{
				if (Exist(i) && f(this->operator[](i), val))
				{
					return i;
				}
			}
			return -1;
		}
	};
}

#endif // !__MANAGEDVECTOR_H__
