#pragma once
#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include"onwind.h"
#include<DirectXMath.h>
#include"Vector.hpp"
#include<cmath>
using namespace DirectX;

namespace AnEngine::DMath
{
	/*class Quaternion
	{
		XMFLOAT4 m_quaternion;
	public:
		Quaternion() = default;
		Quaternion(const Quaternion& q) :m_quaternion(q.m_quaternion)
		{
		}
		Quaternion(float x, float y, float z, float w) :m_quaternion(x, y, z, w)
		{
		}

		/////////////////////////////////////////////////////////////////////////
		inline float X()
		{
			return m_quaternion.x;
		}

		inline void X(float x)
		{
			m_quaternion.x = x;
		}

		inline float Y()
		{
			return m_quaternion.y;
		}

		inline void Y(float y)
		{
			m_quaternion.y = y;
		}

		inline float Z()
		{
			return m_quaternion.z;
		}

		inline void Z(float z)
		{
			m_quaternion.z = z;
		}

		inline float W()
		{
			return m_quaternion.w;
		}

		inline void W(float w)
		{
			m_quaternion.w = w;
		}

		////////////////////////////////////////////////////////////////////////////////////

		Vector3&& Eular()
		{
			Vector3 temp;

			return std::move(temp);
		}

		static Quaternion&& Eular(const Vector3& v)
		{
			//var vv = Vector3::Normalize(v);
			Quaternion temp;
			XMStoreFloat4(&temp.m_quaternion, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&v.m_vector)));
			return std::move(temp);
		}

		static Quaternion&& Eular(float x, float y, float z = 0)
		{
			Quaternion temp;
			XMStoreFloat4(&temp.m_quaternion, XMQuaternionRotationRollPitchYaw(x, y, z));
			return std::move(temp);
		}
	};*/

	using Quaternion = XMVECTOR;
}


#endif // !__QUATERNION_HPP__

