#pragma once
#ifndef __COMPONENTBASE_H__
#define __COMPONENTBASE_H__

#include "Object.h"

namespace AnEngine::Game::ECS
{
	class Entity;

	class ComponentData : public Object
	{
		Entity* m_entity;
	public:
		virtual ~ComponentData() = default;
	};
}
#endif // __COMPONENTBASE_H__

/*
 * 一个Component能且只能属于一个Entity
 */
