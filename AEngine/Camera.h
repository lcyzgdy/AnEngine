#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "ComponentBehaviour.h"

namespace AEngine::Game
{
	class Camera : public ComponentBehaviour
	{
		uint32_t m_cullingMask;
		float m_nearDistance;
		float m_farDistance;
		float m_viewField;

	public:
		Camera() = default;
		~Camera() = default;

		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void BeforeUpdate() override;
		virtual void OnUpdate() override;
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;
	};
}

#endif // !__CAMERA_H__
