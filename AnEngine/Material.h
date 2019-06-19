#pragma once
#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Color.h"
#include "Texture.h"

namespace AnEngine::RenderCore
{
	class Material
	{
		Resource::Color m_mainColor;
		AnEngine::Resource::Texture m_mainTexture;
	};
}

#endif // !__MATERIAL_H__
