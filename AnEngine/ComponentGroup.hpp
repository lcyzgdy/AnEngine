#pragma once
#ifndef __COMPONENTGROUP_HPP__
#define __COMPONENTGROUP_HPP__

#include "ComponentData.h"
#include "Object.h"
#include "onwind.h"

namespace AnEngine::Game
{
	// 保证Index索引的数据指向同一个GameObject
	template <typename T>
	class ComponentGroup : public Object
	{
		uint64_t m_dataType;
		std::deque<T> m_data;

	public:
		ComponentGroup() : m_dataType(typeid(T).hash_code()) {}
		virtual ~ComponentGroup() = default;

		// 如果未来有开发者用这个引擎开发游戏，自己去搞数据同步
		std::deque<T>& Data() { return m_data; }

		T& operator[](int i) { return m_data[i]; }
	};
}  // namespace AnEngine::Game

#endif  // !__COMPONENTGROUP_HPP__
