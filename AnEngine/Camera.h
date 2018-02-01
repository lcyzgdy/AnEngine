#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

//#include "ComponentBehaviour.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "ObjectBehaviour.h"

namespace AnEngine::Game
{
	class Camera : public ObjectBehaviour
	{
	public:
		enum ClearFlags
		{
			SolidColor,
			DepthOnly,
			SkyBox,
			DonotClear,
			ClearFlagCount
		};

	private:
		uint32_t m_cullingMask;
		float m_nearDistance;
		float m_farDistance;
		float m_viewField;

		ClearFlags m_clearFlag;
		Color m_clearColor;

		RenderCore::Resource::ColorBuffer* m_colorBuffer;
		RenderCore::Resource::DepthBuffer* m_depthBuffer;

	protected:
		// 通过 BaseBehaviour 继承
		/*virtual void OnInit() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;
		virtual void OnRelease() override;*/

		virtual void Start() override;
		virtual void OnActive() override;
		virtual void Update() override;
		virtual void AfterUpdate() override;
		virtual void OnInvalid() override;
		virtual void Destory() override;

	public:
		Camera(std::wstring name);
		Camera(std::wstring name, ClearFlags clearFlag);
		~Camera();

		RenderCore::Resource::ColorBuffer* GetColorBuffer();
		RenderCore::Resource::DepthBuffer* GetDepthBuffer();

		void ClearFlag(ClearFlags flag);
		ClearFlags ClearFlag();
		void ClearColor(Color color);
		Color ClearColor();
	};
}

#endif // !__CAMERA_H__
