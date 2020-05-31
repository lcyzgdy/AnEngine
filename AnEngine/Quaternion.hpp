#pragma once
#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include"onwind.h"
#include<DirectXMath.h>
#include"Vector.hpp"
#include<cmath>

namespace AnEngine::DMath
{
	struct Quaternion
	{
		DirectX::XMFLOAT4 m_data;
	public:
		Quaternion() noexcept = default;
		Quaternion(const Quaternion& q) noexcept :m_data(q.m_data) {}
		Quaternion(Quaternion&& q) noexcept :m_data(q.m_data) {}
		Quaternion(float x, float y, float z, float w) noexcept :m_data(x, y, z, w) {}
		Quaternion(const DirectX::XMVECTOR& v) noexcept { DirectX::XMStoreFloat4(&m_data, v); }
		Quaternion(DirectX::XMVECTOR&& v) noexcept { DirectX::XMStoreFloat4(&m_data, v); }

		inline Quaternion& XM_CALLCONV operator=(const DirectX::XMFLOAT4& rhs) noexcept { m_data = rhs; return *this; }
		inline Quaternion& XM_CALLCONV operator=(const Quaternion& rhs) noexcept { m_data = rhs.m_data; return *this; }
		inline Quaternion& XM_CALLCONV operator=(const DirectX::XMVECTOR& v) noexcept { DirectX::XMStoreFloat4(&m_data, v); return *this; }
		inline Quaternion& XM_CALLCONV operator=(DirectX::XMFLOAT4&& rhs) noexcept { m_data = rhs; return *this; }
		inline Quaternion& XM_CALLCONV operator=(Quaternion&& rhs) noexcept { m_data = rhs.m_data; return *this; }
		inline Quaternion& XM_CALLCONV operator=(DirectX::XMVECTOR&& v) noexcept { DirectX::XMStoreFloat4(&m_data, v); return *this; }

		inline operator DirectX::XMFLOAT4() { return m_data; }
		inline operator DirectX::XMVECTOR() { return DirectX::XMLoadFloat4(&m_data); }

		inline DirectX::XMFLOAT4* operator&() { return &m_data; }
		inline const DirectX::XMFLOAT4* operator&() const { return &m_data; }

		/////////////////////////////////////////////////////////////////////////
		inline float X() { return m_data.x; }
		inline void X(float x) { m_data.x = x; }

		inline float Y() { return m_data.y; }
		inline void Y(float y) { m_data.y = y; }

		inline float Z() { return m_data.z; }
		inline void Z(float z) { m_data.z = z; }

		inline float W() { return m_data.w; }
		inline void W(float w) { m_data.w = w; }
		////////////////////////////////////////////////////////////////////////////////////

		static Quaternion&& Eular(const Vector3& v)
		{
			Quaternion temp;
			DirectX::XMStoreFloat4(&temp.m_data,
				DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&v)));
			return std::move(temp);
		}

		static Quaternion&& Eular(float x, float y, float z)
		{
			Quaternion temp;
			DirectX::XMStoreFloat4(&temp.m_data,
				DirectX::XMQuaternionRotationRollPitchYaw(x, y, z));
			return std::move(temp);
		}
	};
}


#endif // !__QUATERNION_HPP__

