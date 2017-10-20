#pragma once
#ifndef __MCAMERA_H__
#define __MCAMERA_H__

#include"onwind.h"
#include"DX.h"
#include"Input.h"
using namespace DirectX;

namespace AEngine
{
	class MCamera
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
		MCamera();
		~MCamera();

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
}
#endif // !__MCAMERA_H__
