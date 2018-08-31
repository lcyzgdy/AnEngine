#pragma once
#ifndef __COMPONENTBASE_H__
#define __COMPONENTBASE_H__

#include "Object.h"

namespace AnEngine::Game
{
	class ComponentData : public Object
	{
	public:
		virtual ~ComponentData() = default;
	};

	template<typename T0, typename... T>
	struct IComponent
	{
		virtual std::tuple<T0*, T*...> operator[](int i) = 0;
	};
}
#endif // __COMPONENTBASE_H__

/*
 * 一个Component能且只能属于一个Entity
 */
