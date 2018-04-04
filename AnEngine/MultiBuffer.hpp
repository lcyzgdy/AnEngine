#pragma once
#ifndef __MULTIBUFFER_H__
#define __MULTIBUFFER_H__

#include "GpuResource.h"

namespace AnEngine::RenderCore::Resource
{
	template<int n, class T>
	class MultiBuffer
	{
		T* m_buffer[n];

	public:
		MultiBuffer()
		{

		}
	};
}

#endif // !__MULTIBUFFER_H__
