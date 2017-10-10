#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include<DirectXMath.h>
using namespace DirectX;

namespace PhysicsCore
{
	class Transform
	{
		XMVECTOR m_position;
		XMVECTOR m_rotation;
		XMVECTOR m_scale;

	public:
		Transform() = default;
		~Transform() = default;

		inline XMVECTOR Position()
		{
			return m_position;
		}

		inline void __vectorcall Position(XMFLOAT3& newPosition)
		{
			//m_position = { newPosition.x, newPosition.y, newPosition.z, 0 };
			m_position = XMVectorSet(newPosition.x, newPosition.y, newPosition.z, 0.0f);
		}

		inline void __vectorcall Position(XMVECTOR& newPosition)
		{
			m_position = newPosition;
		}

		inline float PositionX()
		{
			return m_position.m128_f32[0];
		}

		inline float PositionY()
		{
			return m_position.m128_f32[1];
		}

		inline float PositionZ()
		{
			return m_position.m128_f32[2];
		}
		///////////////////////////////////////////////////////////////
		inline XMVECTOR Rotation()
		{
			return m_rotation;
		}

		inline float RotationX()
		{
			return m_rotation.m128_f32[0];
		}

		inline float RotationY()
		{
			return m_rotation.m128_f32[1];
		}

		inline float RotationZ()
		{
			return m_rotation.m128_f32[2];
		}

		inline float RotationW()
		{
			return m_rotation.m128_f32[3];
		}
	};
}
#endif // !__TRANSFORM_H__
