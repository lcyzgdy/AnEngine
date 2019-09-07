#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "GpuResource.h"

namespace AnEngine::Resource
{
	class Texture
	{
		uint32_t m_width;
		uint32_t m_height;
		DXGI_FORMAT m_format;

	public:
		explicit Texture() = default;
		~Texture() = default;

		inline uint32_t Width() { return m_width; }
		inline uint32_t Height() { return m_height; }
		inline DXGI_FORMAT Format() { return m_format; }
	};
}
#endif