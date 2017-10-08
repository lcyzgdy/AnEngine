#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include<DirectXMath.h>
using namespace DirectX;

namespace PhysicCore
{
	class Transform
	{
		XMVECTOR m_position;
		XMVECTOR m_rotation;
		XMVECTOR m_scale;

	public:
		Transform() = default;
		~Transform() = default;

		XMVECTOR Position()
		{
			return m_position;
		}

		void Position(XMFLOAT3& newPosition)
		{
			m_position = { newPosition.x, newPosition.y, newPosition.z, 0 };
		}

		void Position(XMVECTOR& newPosition)
		{
			m_position = newPosition;
		}

		float PositionX()
		{
			return m_position.m128_f32[0];
		}

		float PositionY()
		{
			return m_position.m128_f32[1];
		}

		float PositionZ()
		{
			return m_position.m128_f32[2];
		}
		///////////////////////////////////////////////////////////////
		XMVECTOR Rotation()
		{
			return m_rotation;
		}

		float RotationX()
		{
			return m_rotation.m128_f32[0];
		}

		float RotationY()
		{
			return m_rotation.m128_f32[1];
		}

		float RotationZ()
		{
			return m_rotation.m128_f32[2];
		}

		float RotationW()
		{
			return m_rotation.m128_f32[3];
		}
	};
}
#endif // !__TRANSFORM_H__
