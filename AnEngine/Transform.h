#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include<DirectXMath.h>
#include"Vector.hpp"
#include"Quaternion.hpp"

using namespace DirectX;
using namespace AnEngine::Math;

namespace AnEngine::Game
{
	class Transform
	{
		Vector3 m_position;
		Quaternion m_rotation;
		Vector3 m_scale;

	public:
		Transform() = default;
		~Transform() = default;

		inline Vector3 Position()
		{
			return m_position;
		}

		inline void Position(const Vector3& newPosition)
		{
			m_position = newPosition;
		}
		///////////////////////////////////////////////////////////////
		inline Quaternion Rotation()
		{
			return m_rotation;
		}

		XMFLOAT4X4 GetModelMatrix()
		{
			XMFLOAT4X4 t(1, 0, 0, m_position.X(),
				0, 1, 0, m_position.Y(),
				0, 0, 1, m_position.Z(),
				0, 0, 0, 1);
			return XMFLOAT4X4();
		}

		Transform& operator=(const Transform& t)
		{
			m_position = t.m_position;
			m_rotation = t.m_rotation;
			m_scale = t.m_scale;
			return *this;
		}
	};
}
#endif // !__TRANSFORM_H__
