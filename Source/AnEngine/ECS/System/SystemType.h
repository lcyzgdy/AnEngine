#pragma once
#ifndef __ANENGINE_SYSTEMTYPE_H__
#define __ANENGINE_SYSTEMTYPE_H__

namespace AnEngine::ECS
{
	enum class SystemType
	{
		/// <summary>
		/// 创建型System，仅运行一次
		/// </summary>
		Create,
		/// <summary>
		/// 行为型，循环执行
		/// </summary>
		Behaviour,
	};
}

#endif // !__ANENGINE_SYSTEMTYPE_H__
