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
		DMath::Vector3 m_localPosition;
		DMath::Quaternion m_localRotation;
		DMath::Vector3 m_localScale;

		Transform* m_parent;

		DMath::SMatrix4x4 ObjectToLocalMatrix()
		{
			var T = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&m_localPosition));
			var R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&m_localRotation));
			var S = DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&m_localScale));
			return T * R * S;
		}

		DMath::SMatrix4x4 LocalToWorldMatrix()
		{
			DMath::SMatrix4x4 local = DirectX::XMMatrixIdentity();
			var parent = m_parent;
			while (parent != nullptr)
			{
				var S = DirectX::XMMatrixScalingFromVector(parent->LocalScale());
				var T = DirectX::XMMatrixTranslationFromVector(parent->LocalPosition());
				var R = DirectX::XMMatrixRotationQuaternion(parent->LocalRotation());

				local *= T * R * S;
				parent = parent->m_parent;
			}
			return local;
		}

	public:
		Transform() = default;
		virtual ~Transform() = default;

		inline DMath::SVector3 Position() { return XMVector3Transform(XMLoadFloat3(&m_localPosition), LocalToWorldMatrix()); }
		// TODO: Set from world position
		inline DMath::SVector3 LocalPosition() { return XMLoadFloat3(&m_localPosition); }
		inline void LocalPosition(const DMath::Vector3& newLocalPos) { m_localPosition = newLocalPos; }
		inline void LocalPosition(DMath::Vector3&& newLocalPos) { m_localPosition = newLocalPos; }
		inline void LocalPosition(const DMath::SVector3& newLocalPos) { DirectX::XMStoreFloat3(&m_localPosition, newLocalPos); }
		inline void LocalPosition(DMath::SVector3&& newLocalPos) { DirectX::XMStoreFloat3(&m_localPosition, newLocalPos); }




		inline DMath::SQuaternion Rotation() { return DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_localRotation), LocalToWorldMatrix()); }
		// TODO: Set from world rotation
		inline DMath::SQuaternion LocalRotation() { return DirectX::XMLoadFloat4(&m_localRotation); }
		inline void LocalRotation(const DMath::Quaternion& newRot) { m_localRotation = newRot; }
		inline void LocalRotation(DMath::Quaternion&& newRot) { m_localRotation = newRot; }
		inline void LocalRotation(const DMath::SQuaternion& newRot) { DirectX::XMStoreFloat4(&m_localRotation, newRot); }
		inline void LocalRotation(DMath::SQuaternion&& newRot) { DirectX::XMStoreFloat4(&m_localRotation, newRot); }



		DMath::SVector3 LocalScale() { return DirectX::XMLoadFloat3(&m_localScale); }
		void LocalScale(const DMath::SVector3& newScale) { DirectX::XMStoreFloat3(&m_localScale, newScale); }

		DMath::SMatrix4x4 ObjectToWorldMatrix() { return ObjectToLocalMatrix() * LocalToWorldMatrix(); }

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