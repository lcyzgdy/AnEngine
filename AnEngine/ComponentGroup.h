#pragma once
#ifndef __COMPONENTGROUP_HPP__
#define __COMPONENTGROUP_HPP__

#include "onwind.h"
#include "Object.h"
#include "ComponentData.h"

namespace AnEngine::Game
{
	template<typename T>
	class ComponentGroup : public Object
	{
		uint64_t m_dataType;
		std::vector<T> m_data;

	public:
		ComponentGroup() : m_dataType(typeid(T).hash_code())
		{
			if (!IsDerived<T, ComponentData>::Result) throw std::exception("类型不对");
		}
		virtual ~ComponentGroup = default;
	};
}

#endif // !__COMPONENTGROUP_HPP__
