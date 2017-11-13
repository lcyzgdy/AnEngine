#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include<DirectXMath.h>
#include"Vector.hpp"
#include"Quaternion.hpp"

using namespace DirectX;
using namespace AEngine::Math;

namespace AEngine::PhysicsCore
{
	class Transform
	{
		Vector3 m_position;
		Quaternion m_rotation;
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
			m_position = newPosition;
		}
		///////////////////////////////////////////////////////////////
		__FasterFunc(Quaternion) Rotation()
		{
			return m_rotation;
		}

		Transform& operator=(const Transform& t)
		{
			m_position = t.m_position;
			m_rotation = t.m_rotation;
			m_scale = t.m_scale;
		}
	};
}
#endif // !__TRANSFORM_H__
