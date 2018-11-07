#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Quaternion.hpp"
#include <mutex>

namespace AnEngine::Game
{
	class Transform
	{
		//DMath::Vector3 m_position;
		//DMath::Quaternion m_rotation;
		//DMath::Vector3 m_scale;

		DMath::Vector3 m_localPosition;
		DMath::Quaternion m_localRotation;
		DMath::Vector3 m_localScale;

		Transform* m_parent;

		DMath::Matrix4x4 ObjectToLocalMatrix()
		{
			var T = XMMatrixTranslationFromVector(XMLoadFloat3(&m_localPosition));
			var R = XMMatrixRotationQuaternion(m_localRotation);
			var S = XMMatrixScalingFromVector(XMLoadFloat3(&m_localScale));
			return T * R * S;
		}

		DMath::Matrix4x4 LocalToWorldMatrix()
		{
			DMath::Matrix4x4 local = XMMatrixIdentity();
			var parent = m_parent;
			while (parent != nullptr)
			{
				var S = XMMatrixScalingFromVector(XMLoadFloat3(&(parent->LocalScale())));
				var T = XMMatrixTranslationFromVector(XMLoadFloat3(&(parent->LocalPosition())));
				var R = XMMatrixRotationQuaternion(parent->LocalRotation());
				local *= T * R * S;
				parent = parent->m_parent;
			}
			return local;
		}

	public:
		Transform() = default;
		virtual ~Transform() = default;

		DMath::Vector3 Position()
		{
			DMath::Vector3 ret;
			XMStoreFloat3(&ret, XMVector3Transform(XMLoadFloat3(&m_localPosition), LocalToWorldMatrix()));
			return ret;
		}

		//void Position(const DMath::Vector3& newPosition) { m_position = newPosition; }

		DMath::Vector3 LocalPosition() { return m_localPosition; }

		void LocalPosition(const DMath::Vector3& newLocalPos) { m_localPosition = newLocalPos; }

		DMath::Quaternion Rotation() { return XMVector3Transform(m_localRotation, LocalToWorldMatrix()); }

		//void Rotation(const DMath::Quaternion& newRotation) { m_rotation = newRotation; }

		DMath::Quaternion LocalRotation() { return m_localRotation; }

		void LocalRotation(const DMath::Quaternion newRot) { m_localRotation = newRot; }

		//DMath::Vector3 Scale() { return m_scale; }

		//void Scale(const DMath::Vector3& newScale) { m_scale = newScale; }

		DMath::Vector3 LocalScale() { return m_localScale; }

		void LocalScale(const DMath::Vector3 newScale) { m_localScale = newScale; }

		DMath::Matrix4x4 ObjectToWorldMatrix() { return ObjectToLocalMatrix() * LocalToWorldMatrix(); }

		DMath::Matrix4x4 WorldToObjectMatrix();
	};

	/*using Position = DMath::Vector3;
	using Rotation = DMath::Quaternion;
	using Scale = DMath::Vector3;
	using LocalPosition = DMath::Vector3;
	using LocalRotation = DMath::Quaternion;
	using LocalScale = DMath::Vector3;*/
	using TransformMatrix = DMath::Matrix4x4;
}

namespace AnEngine::Game::Component
{
	using Position = DMath::Vector3;
	using Rotation = DMath::Quaternion;
	using Scale = DMath::Vector3;

	using Matrix4x4 = DMath::Matrix4x4;
}

#endif // !__TRANSFORM_H__