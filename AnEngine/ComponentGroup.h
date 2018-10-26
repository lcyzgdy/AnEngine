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
		}
		virtual ~ComponentGroup = default;

		// 开发者自己去搞数据同步，我被这个问题折腾好几天了
		std::vector<T>& Data() { return m_data; }
	};
}

#endif // !__COMPONENTGROUP_HPP__
