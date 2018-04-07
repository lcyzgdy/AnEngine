#pragma once
#ifndef __MULTIBUFFER_H__
#define __MULTIBUFFER_H__

#include "ColorBuffer.h"
#include "Fence.hpp"

namespace AnEngine::RenderCore::Resource
{
	template<int N, class T>
	class MultiBuffer
	{
		T* m_buffer[N];
		uint32_t m_index;
		bool m_ok;

		Fence* m_fence;
		uint64_t m_fenceValues[N];
		HANDLE m_fenceEvent;

	public:
		MultiBuffer() : m_index(0), m_ok(false)
		{
		}

		~MultiBuffer()
		{
			delete[] m_buffer;
		}

		void ManageBuffer(T* buffer)
		{
			if (m_ok) throw MultiBufferFullException();
			m_buffer[m_index] = buffer;
			if (m_index == N - 1)
			{
				m_ok = true;
				m_index = 0;
				return;
			}
			m_index++;
		}

		void Swap()
		{
			m_index++;
			m_index %= N;
		}

		T* GetFront()
		{
			return m_buffer[m_index];
		}

		T* GetBack()
		{
			return m_buffer[(m_index + 1) % N];
		}
	};

#include<exception>

	class MultiBufferFullException : std::exception
	{
	public:
		MultiBufferFullException() : exception("Multi buffer is full")
		{
		}
	};
}

#endif // !__MULTIBUFFER_H__
