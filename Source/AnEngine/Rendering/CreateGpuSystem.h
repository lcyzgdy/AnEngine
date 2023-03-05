#pragma once
#ifndef __ANENGINE__CREATEGPUSYSTEM_H__
#define __ANENGINE__CREATEGPUSYSTEM_H__

#include "../ECS/System/SystemBase.hpp"
#include "../ECS/Component/Parameter.hpp"

namespace AnEngine::Rendering
{
	class CreateGpuDeviceSystem : public ECS::SystemBaseGeneric<ECS::SystemType::Create, ECS::ReadOnlyParameter<int>>
	{
	public:
		virtual void CreateSystem() override
		{

		}

		virtual void OnUpdate(const int& t) override
		{
		}
	};
}
#endif // !__ANENGINE__CREATEGPUSYSTEM_H__