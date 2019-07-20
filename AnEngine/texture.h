#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "GpuResource.h"

namespace AnEngine::Resource
{
	class Texture : public RenderCore::Resource::GpuResource
	{
	public:
		explicit Texture();
		~Texture() = default;
	};
}
#endif