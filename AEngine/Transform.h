#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include<DirectXMath.h>
#include"Vector.hpp"
using namespace DirectX;
using namespace Math;

namespace PhysicsCore
{
	class Transform
	{
		Vector3 m_position;
		XMVECTOR m_rotation;
		Vector3 m_scale;

	public:
		Transform() = default;
		~Transform() = default;

		__FasterFunc(Vector3) Position()
		{
			return m_position;
		}

		__FasterFunc(void) Position(const Vector3& newPosition)
		{
			//m_position = { newPosition.x, newPosition.y, newPosition.z, 0 };
			m_position = newPosition;
		}
		///////////////////////////////////////////////////////////////
		inline XMVECTOR Rotation()
		{
			return m_rotation;
		}
	};
}
#endif // !__TRANSFORM_H__
