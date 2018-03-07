#pragma once
#ifndef __MCAMERA_H__
#define __MCAMERA_H__

#include"onwind.h"
#include"DX.h"
#include"Input.h"
using namespace DirectX;

namespace AnEngine
{
	class MiCamera
	{
		struct KeyPressed
		{
			bool w, a, s, d, up, left, down, right;
		};
		XMFLOAT3 initialPosition;	// 初始位置
		XMFLOAT3 position;
		float yawRotate;	// 水平转动
		float pitchRotate;	// 垂直转动
		XMFLOAT3 lookDirection;
		XMFLOAT3 upDirection;
		float moveSpeed;	// 相机移动速度
		float turnSpeed;	// 相机转动速度
		KeyPressed keyPressed;

		void Reset();

	public:
		MiCamera();
		~MiCamera();

		void OnInit(XMFLOAT3 _position);
		void OnUpdate(float _elapsedSeconds);
		void OnRelease();

		void OnKeyUp(UINT8 _key);
		void OnKeyDown(UINT8 _key);

		XMMATRIX GetViewMatrix();
		XMMATRIX GetProjectionMatrix(float _fov, float _aspectRatio, float _nearPlane = 1.0f, float _farPlane = 1000.0f);
		void SetMoveSpeed(float v);// 单位：uint/s
		void SetTurnSpeed(float v);// 单位：s^-1 || /s
	};

	class LightCamera
	{
	public:
		LightCamera();
		~LightCamera();

		void Get3DViewProjMatrices(XMFLOAT4X4 *view, XMFLOAT4X4 *proj, float fovInDegrees, float screenWidth, float screenHeight);
		void Reset();
		void Set(XMVECTOR eye, XMVECTOR at, XMVECTOR up);
		static LightCamera *get();
		void RotateYaw(float deg);
		void RotatePitch(float deg);
		void GetOrthoProjMatrices(XMFLOAT4X4 *view, XMFLOAT4X4 *proj, float width, float height);
		XMVECTOR m_eye;
		XMVECTOR m_at;
		XMVECTOR m_up;
	private:
		static LightCamera* m_camera;
	};
}
#endif // !__MCAMERA_H__
