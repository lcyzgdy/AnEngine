#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

//#include "ComponentBehaviour.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "ObjectBehaviour.h"

namespace AEngine::Game
{
	class Camera : public ObjectBehaviour
	{
		uint32_t m_cullingMask;
		float m_nearDistance;
		float m_farDistance;
		float m_viewField;

		RenderCore::Resource::ColorBuffer* m_colorBuffer;
		RenderCore::Resource::DepthBuffer* m_depthBuffer;

	protected:
		// 通过 BaseBehaviour 继承
		virtual void OnInit() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;

	public:
		Camera();
		~Camera() = default;

		RenderCore::Resource::ColorBuffer* GetColorBuffer();
		RenderCore::Resource::DepthBuffer* GetDepthBuffer();
	};
}

#endif // !__CAMERA_H__
