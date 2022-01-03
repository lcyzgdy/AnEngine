#pragma once
#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include <DirectXMath.h>

#include "onwind.h"
#include "Vector.hpp"

namespace AnEngine::DMath
{
	struct DLL_API Matrix4x4
	{
	private:
		DirectX::XMFLOAT4X4 m_data;
	public:
		Matrix4x4() noexcept = default;

		Matrix4x4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) noexcept : m_data(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33)
		{}
		Matrix4x4(const Matrix4x4& m) noexcept :m_data(m.m_data) {}
		Matrix4x4(Matrix4x4&& m) noexcept :m_data(m.m_data) {}
		explicit Matrix4x4(_In_reads_(16) const float* pArray) noexcept : m_data(pArray) {}
		Matrix4x4(const DirectX::XMFLOAT4X4& m) noexcept :m_data(m) {}
		Matrix4x4(DirectX::XMFLOAT4X4&& m) noexcept :m_data(m) {}
		Matrix4x4(const DirectX::XMMATRIX& m) noexcept { DirectX::XMStoreFloat4x4(&m_data, m); }
		Matrix4x4(DirectX::XMMATRIX&& m) noexcept { DirectX::XMStoreFloat4x4(&m_data, m); }

		inline operator DirectX::XMFLOAT4X4() { return m_data; }
		inline operator DirectX::XMMATRIX() { return DirectX::XMLoadFloat4x4(&m_data); }

		Matrix4x4& XM_CALLCONV operator= (const Matrix4x4& M) noexcept { m_data = M.m_data; return *this; }
		Matrix4x4& XM_CALLCONV operator= (Matrix4x4&& M) noexcept { m_data = M.m_data; return *this; }

		Matrix4x4 operator*(const Matrix4x4& rhs) noexcept { return DirectX::XMMatrixMultiply(*this, const_cast<Matrix4x4&>(rhs)); }
		Matrix4x4 operator*(Matrix4x4&& rhs) noexcept { return DirectX::XMMatrixMultiply(*this, rhs); }

		Matrix4x4& operator*=(const Matrix4x4& rhs) noexcept { DirectX::XMStoreFloat4x4(&m_data, DirectX::XMMatrixMultiply(*this, const_cast<Matrix4x4&>(rhs))); return *this; }

		Matrix4x4 Inverse() noexcept { return DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&m_data)); }
	};
}

#endif // __MATRIX_HPP__
