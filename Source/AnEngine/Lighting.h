#pragma once
#ifndef __LIGHTING_H__
#define __LIGHTING_H__

#include "ObjectBehaviour.h"
#include "LightingType.h"

namespace AnEngine::Game
{
	class LightingBase
	{
		AnEngine::RenderCore::LightingType m_type;

	public:
		LightingBase(RenderCore::LightingType type) : m_type(type) {}
	};
}
#endif // !__LIGHTING_H__
