#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <mutex>
#include <vector>

#include "Component.h"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Quaternion.hpp"


namespace AnEngine::Game
{
	class Transform : public Component
	{
		DMath::Vector3 m_localPosition;
		DMath::Quaternion m_localRotation;
		DMath::Vector3 m_localScale;

		Transform* m_parent;
		std::vector<Transform*> m_children;

		inline DMath::Matrix4x4 LocalMatrix() const
		{
			return DirectX::XMMatrixAffineTransformation(m_localScale, DMath::Vector3::Zero, m_localRotation, m_localPosition);
			// var T = DirectX::XMMatrixTranslationFromVector(m_localPosition);
			// var R = DirectX::XMMatrixRotationQuaternion(m_localRotation);
			// var S = DirectX::XMMatrixScalingFromVector(m_localScale);
			// return T * R * S;
		}

		inline DMath::Matrix4x4 LocalToWorldMatrix() const
		{
			DMath::Matrix4x4 local = DirectX::XMMatrixIdentity();
			var parent = m_parent;
			while (parent != nullptr)
			{
				// local *= DirectX::XMMatrixAffineTransformation(parent->m_localScale, DMath::Vector3::Zero, parent->m_localRotation, parent->m_localPosition);
				local *= parent->LocalMatrix();
				// var T = DirectX::XMMatrixTranslationFromVector(const_cast<DMath::Vector3&>(parent->LocalPosition()));
				// var R = DirectX::XMMatrixRotationQuaternion(const_cast<DMath::Quaternion&>(parent->LocalRotation()));
				// var S = DirectX::XMMatrixScalingFromVector(const_cast<DMath::Vector3&>(parent->LocalScale()));
				// local *= T * R * S;
				parent = parent->m_parent;
			}
			return local;
		}

	public:
		Transform() : m_localPosition(), m_localRotation(), m_localScale(1.0f, 1.0f, 1.0f), m_parent(nullptr) {};
		virtual ~Transform() = default;

		inline DMath::Vector3 Position() { return DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_localPosition), LocalToWorldMatrix()); }
		// TODO: Set from world position
		inline const DMath::Vector3& LocalPosition() const { return XMLoadFloat3(&m_localPosition); }
		inline void LocalPosition(const DMath::Vector3& newLocalPos) { m_localPosition = newLocalPos; }
		inline void LocalPosition(DMath::Vector3&& newLocalPos) { m_localPosition = newLocalPos; }
		// inline void LocalPosition(const DMath::SVector3& newLocalPos) { DirectX::XMStoreFloat3(&m_localPosition, newLocalPos); }
		// inline void LocalPosition(DMath::SVector3&& newLocalPos) { DirectX::XMStoreFloat3(&m_localPosition, newLocalPos); }



		inline const DMath::Quaternion Rotation() const { return DirectX::XMVector4Transform(m_localRotation, LocalToWorldMatrix()); }
		// TODO: Set from world rotation               
		// inline DMath::Quaternion LocalRotation() { return m_localRotation; }
		inline const DMath::Quaternion& LocalRotation() const { return m_localRotation; }
		inline void LocalRotation(const DMath::Quaternion& newRot) { m_localRotation = newRot; }
		inline void LocalRotation(DMath::Quaternion&& newRot) { m_localRotation = newRot; }
		// inline void LocalRotation(const DMath::Quaternion& newRot) { m_localRotation = newRot; }
		// inline void LocalRotation(DMath::Quaternion&& newRot) { DirectX::XMStoreFloat4(&m_localRotation, newRot); }



		inline const DMath::Vector3& LocalScale() const { return m_localScale; }
		inline void LocalScale(const DMath::Vector3& newScale) { m_localScale = newScale; }

		DMath::Matrix4x4 ObjectToWorldMatrix() { return LocalMatrix() * LocalToWorldMatrix(); }

		DMath::Matrix4x4 WorldToObjectMatrix() { return ObjectToWorldMatrix().Inverse(); }

		inline Transform* Parent() const { return m_parent; }
		inline Transform* Parent(Transform* parent) { m_parent = parent; }

		inline const std::vector<Transform*>& Children() const { return m_children; }

		inline void AddChild(Transform* child)
		{
			m_children.push_back(child);
			child->m_parent = this;
		}
	};
}

#endif // !__TRANSFORM_H__