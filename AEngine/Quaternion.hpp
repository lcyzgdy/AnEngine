#pragma once
#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include"../Utility/onwind.h"
#include<DirectXMath.h>
using namespace DirectX;

namespace Math
{
	class Quaternion
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
		__FasterFunc(float) X()
		{
			return m_quaternion.x;
		}

		__FasterFunc(void) X(float x)
		{
			m_quaternion.x = x;
		}

	};
}


#endif // !__QUATERNION_HPP__

