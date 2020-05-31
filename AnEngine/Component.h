#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <mutex>
#include <shared_mutex>

namespace AnEngine::Game
{
	class GameObject;


	/*
	 * Component 用于存放数据，且仅存放数据，可多线程读和单线程写
	 */
	class Component
	{
		GameObject* m_entity;
		std::shared_mutex m_mutex;
	public:
		virtual ~Component() = default;
	};
}
#endif // __COMPONENT_H__

/*
 * 一个Component能且只能属于一个Entity
 */
