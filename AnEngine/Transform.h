#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Quaternion.hpp"
#include "ComponentBase.h"

namespace AnEngine::Game
{
	class Transform : ComponentBase
	{
		DMath::Vector3 m_position;
		DMath::Quaternion m_rotation;
		DMath::Vector3 m_scale;

		DMath::Vector3 m_localPosition;
		DMath::Quaternion m_localRotation;
		DMath::Vector3 m_localScale;

		Transform* m_parent;

	public:
		Transform() = default;
		virtual ~Transform() = default;

		DMath::Vector3 Position()
		{
			return m_position;
		}

		void Position(const DMath::Vector3& newPosition)
		{
			m_position = newPosition;
		}

		DMath::Vector3 LocalPosition()
		{
			return m_localPosition;
		}

		void LocakPosition(const DMath::Vector3& newLocalPos)
		{
			m_localPosition = newLocalPos;
		}

		DMath::Quaternion Rotation()
		{
			return m_rotation;
		}

		void Rotation(const DMath::Quaternion& newRotation)
		{
			m_rotation = newRotation;
		}

		DMath::Quaternion LocalRotation()
		{
			return m_localRotation;
		}

		void LocalRotation(const DMath::Quaternion newRot)
		{
			m_localRotation = newRot;
		}

		DMath::Vector3 Scale()
		{
			return m_scale;
		}

		void Scale(const DMath::Vector3& newScale)
		{
			m_scale = newScale;
		}

		DMath::Vector3 LocalScale()
		{
			return m_localScale;
		}

		void LocalScale(const DMath::Vector3 newScale)
		{
			m_localScale = newScale;
		}

		DMath::Matrix4x4 ObjectToWorldMatrix()
		{
			DMath::Matrix4x4 local(cosf(m_rotation.X()), 0, 0, m_localPosition.X(),
				0, 0, 0, m_localPosition.Y(),
				0, 0, 0, m_localPosition.Z(),
				m_localScale.X(), m_localScale.Y(), m_localScale.Z(), 1);
			var parent = m_parent;
			while (parent != nullptr)
			{
				local *= parent->ObjectToWorldMatrix();
				parent = parent->m_parent;
			}
			return local;
		}
	};
}
#endif // !__TRANSFORM_H__
