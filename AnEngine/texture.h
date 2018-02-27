#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "GpuResource.h"

namespace AnEngine::RenderCore::Resource
{
	class Texture : public GpuResource
	{
	public:
		explicit Texture();
		~Texture() = default;
	};
}
#endif