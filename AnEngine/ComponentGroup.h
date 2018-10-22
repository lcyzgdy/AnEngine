#pragma once
#ifndef __COMPONENTGROUP_HPP__
#define __COMPONENTGROUP_HPP__

#include "onwind.h"
#include "Object.h"
#include "ComponentData.h"

namespace AnEngine::Game::ECS
{
	template<typename T>
	class ComponentGroup : public Object
	{
		type_info m_dataType;
		std::vector<T> m_data;

	public:
		ComponentGroup() : m_dataType(typeid(T))
		{
			if (!IsDerived<T, ComponentData>::Result) throw std::exception("类型不对");
		}
		virtual ~ComponentGroup = default;
	};
}

#endif // !__COMPONENTGROUP_HPP__
