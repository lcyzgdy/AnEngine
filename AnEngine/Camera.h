#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

//#include "ComponentBehaviour.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "ObjectBehaviour.h"
#include "ShaderClass.h"
#include "MultiBuffer.hpp"

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

		//RenderCore::Resource::ColorBuffer* m_colorBuffer;
		//RenderCore::Resource::DepthBuffer* m_depthBuffer;

		RenderCore::Resource::MultiBuffer<2, RenderCore::Resource::ColorBuffer> m_colorBuffers;
		RenderCore::Resource::MultiBuffer<2, RenderCore::Resource::DepthBuffer> m_depthBuffers;

		std::mutex m_rtvMutex;

		RenderCore::PixelShader* m_postProcessShader;

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
		virtual void LateUpdate() override;
		virtual void OnInvalid() override;
		virtual void Destory() override;

		void PostProcess();

	public:
		/*Camera(const std::wstring& name);
		Camera(std::wstring&& name);
		Camera(const std::wstring& name, ClearFlags clearFlag);
		Camera(std::wstring&& name, ClearFlags clearFlag);*/
		Camera();
		Camera(ClearFlags clearFlag);
		~Camera();

		RenderCore::Resource::ColorBuffer* GetColorBuffer();
		RenderCore::Resource::DepthBuffer* GetDepthBuffer();

		void ClearFlag(ClearFlags flag);
		ClearFlags ClearFlag();
		void ClearColor(Color color);
		Color ClearColor();

		static ColorBuffer* FindForwordTarget(Vector3&& pos);
	};
}

#endif // !__CAMERA_H__
