﻿#pragma once
#ifndef __MULTIBUFFER_H__
#define __MULTIBUFFER_H__

#include "ColorBuffer.h"
#include "Fence.h"
#include<exception>

namespace AnEngine::RenderCore::Resource
{
	class MultiBufferFullException : std::exception
	{
	public:
		MultiBufferFullException() : exception("Multi buffer is full")
		{
		}
	};

	template<int _N, class _Ty>
	class MultiBuffer
	{
		_Ty* m_buffer[_N];
		uint32_t m_index;
		bool m_ok;

	public:
		MultiBuffer() : m_index(0), m_ok(false)
		{
		}

		~MultiBuffer()
		{
			delete[] m_buffer;
		}

		void ManageBuffer(_Ty* buffer)
		{
			if (m_ok)
			{
				throw MultiBufferFullException();
			}
			m_buffer[m_index] = buffer;
			if (m_index == _N - 1)
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
			m_index %= _N;
		}

		_Ty* GetFront()
		{
			return m_buffer[m_index];
		}

		_Ty* GetBack()
		{
			return m_buffer[(m_index + 1) % _N];
		}
	};
}

#endif // !__MULTIBUFFER_H__
