#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <mutex>
#include <shared_mutex>

namespace AnEngine::Game
{
	class GameObject;


	/*
	 * ComponentBase 用于存放数据，且仅存放数据，可多线程读和单线程写
	 * 暂时可放获取数据的逻辑？？？？？？？
	 * 必须拥有无参构造函数
	 */
	class ComponentBase
	{
		friend class GameObject;
	protected:
		GameObject* m_entity;
		std::shared_mutex m_mutex;
	public:
		virtual ~ComponentBase() = default;
	};
}
#endif // __COMPONENT_H__

/*
 * 一个Component能且只能属于一个Entity
 */
