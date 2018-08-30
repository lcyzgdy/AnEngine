#pragma once
#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include "onwind.h"
#include "Vector.hpp"
#include<DirectXMath.h>

namespace AnEngine::DMath
{
	/*struct Matrix4x4
	{
	private:
		//DirectX::XMFLOAT4X4
		DirectX::XMMATRIX r;
	public:
		Matrix4x4() = default;

		Matrix4x4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) : r(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33)
		{}
		Matrix4x4(const Matrix4x4& m) : r(m.r) {}
		explicit Matrix4x4(_In_reads_(16) const float *pArray) : r(pArray) {}

		Matrix4x4& XM_CALLCONV operator= (Matrix4x4& M)
		{
			r = M.r;
			return *this;
		}
		Matrix4x4& XM_CALLCONV operator+= (Matrix4x4& M)
		{
			r += M.r;
			return *this;
		}
		Matrix4x4& XM_CALLCONV operator-= (Matrix4x4& M)
		{
			r -= M.r;
			return *this;
		}
		Matrix4x4& XM_CALLCONV operator*= (Matrix4x4& M)
		{
			r *= M.r;
			return *this;
		}
		Matrix4x4& operator*= (float S)
		{
			r *= S;
			return *this;
		}
		Matrix4x4& operator/= (float S)
		{
			r /= S;
			return *this;
		}

		Matrix4x4 XM_CALLCONV operator+ (Matrix4x4& M) const;
		Matrix4x4 XM_CALLCONV operator- (Matrix4x4& M) const;
		Matrix4x4 XM_CALLCONV operator* (Matrix4x4& M) const;
		Matrix4x4 operator* (float S) const;
		Matrix4x4 operator/ (float S) const;
	};*/


	using Matrix4x4 = DirectX::XMMATRIX;
}

#endif // __MATRIX_HPP__
