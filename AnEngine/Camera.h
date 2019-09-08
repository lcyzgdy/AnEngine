#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "ObjectBehaviour.h"
#include "ShaderClass.h"
#include "MultiBuffer.hpp"
#include "DMath.hpp"
#include "Matrix.hpp"

namespace AnEngine::Game
{
	class DLL_API Camera : public ObjectBehaviour
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

		DMath::Matrix4x4 m_viewMatrix;
		DMath::Matrix4x4 m_projectionMatrix;
		DMath::Matrix4x4 m_nonJitteredProjectionMatrix;

		ClearFlags m_clearFlag;
		Color m_clearColor;

		RenderCore::Resource::MultiBuffer<2, RenderCore::Resource::ColorBuffer> m_colorBuffers;
		RenderCore::Resource::MultiBuffer<2, RenderCore::Resource::DepthBuffer> m_depthBuffers;

		std::mutex m_rtvMutex;

		RenderCore::PixelShader* m_postProcessShader;

	protected:
		virtual void Start() override;
		virtual void OnActive() override;
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void OnInvalid() override;
		virtual void Destory() override;

		void PostProcess();

	public:
		Camera();
		Camera(ClearFlags clearFlag);
		~Camera();

		RenderCore::Resource::ColorBuffer* GetColorBuffer() { return m_colorBuffers.GetBack(); }
		RenderCore::Resource::DepthBuffer* GetDepthBuffer() { return m_depthBuffers.GetBack(); }

		void ClearFlag(ClearFlags flag);
		ClearFlags ClearFlag() { return m_clearFlag; }
		void ClearColor(Color color) { m_clearColor = color; }
		Color ClearColor() { return m_clearColor; }

		static ColorBuffer* FindForwordTarget(DMath::Vector3&& pos);

		DMath::Matrix4x4& ViewMatrix() { return m_viewMatrix; }
		DMath::Matrix4x4& ProjectionMatrix() { return m_projectionMatrix; }
		DMath::Matrix4x4& NonJitteredProjectionMatrix() { return m_nonJitteredProjectionMatrix; }
		void ResetProjectionMatrix();
		void ViewMatrix(DMath::Matrix4x4& v) { m_viewMatrix = v; }
		void ProjectionMatrix(DMath::Matrix4x4& p) { m_projectionMatrix = p; }
		void NonJitteredProjectionMatrix(DMath::Matrix4x4& njp) { m_nonJitteredProjectionMatrix = njp; }
	};
}

#endif // !__CAMERA_H__
