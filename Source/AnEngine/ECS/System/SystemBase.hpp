#pragma once
#ifndef __ANENGINE_SYSTEMBASE_H__
#define __ANENGINE_SYSTEMBASE_H__

#include <type_traits>

#include "SystemType.h"

namespace AnEngine::ECS
{
	class SystemBase
	{
	public:
		virtual void CreateSystem()
		{
		}
	};

	template<SystemType _SystemType, typename ... _ComponentParameterType>
	class SystemBaseGeneric : public SystemBase
	{
		virtual void OnUpdate(_ComponentParameterType::_RealType ... args) = 0;
	public:
		inline static constexpr SystemType GetSystemType() { return _SystemType; }
	};
}

#endif // !__ANENGINE_SYSTEMBASE_H__
