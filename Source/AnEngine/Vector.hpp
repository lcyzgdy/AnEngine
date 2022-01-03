#pragma once
#ifndef __AENGEIN_VECTOR_HPP__
#define __AENGINE_VECTOR_HPP__

#include"onwind.h"
#include<DirectXMath.h>

/*(http://blog.csdn.net/zjjzhaohang/article/details/68936082)
为了效率起见，在DX12中传参XMVECTOR 时，有一定规则的调用方式，以便利用SSE/SSE2硬件加速。
1、前三个XMVECTOR 参数调用时，应该使用FXMVECTOR
2、第四个XMVECTOR参数，应该使用GXMVECTOR
3、第五、六个XMVECTOR参数，应该使用HXMVECTOR
4、剩下的额外的参数使用CXMVECTOR
5、若其中穿插了其他类型参数，规则忽略其他参数照样生效
*/

namespace AnEngine::DMath
{
	struct Vector3
	{
		DirectX::XMFLOAT3 m_data;

	public:
		Vector3() noexcept = default;
		Vector3(const Vector3& rhs) noexcept :m_data(rhs.m_data) {}
		Vector3(Vector3&& rhs) noexcept :m_data(rhs.m_data) {}
		Vector3(float x, float y, float z) noexcept :m_data(x, y, z) {}
		Vector3(const DirectX::XMVECTOR& v) noexcept { DirectX::XMStoreFloat3(&m_data, v); }
		Vector3(DirectX::XMVECTOR&& v) noexcept { DirectX::XMStoreFloat3(&m_data, v); }

		inline Vector3& operator=(const DirectX::XMFLOAT3& rhs) noexcept { m_data = rhs; return *this; }
		inline Vector3& operator=(const Vector3& rhs) noexcept { m_data = rhs.m_data; return *this; }
		inline Vector3& operator=(const DirectX::XMVECTOR& v) noexcept { DirectX::XMStoreFloat3(&m_data, v); return *this; }
		inline Vector3& operator=(DirectX::XMFLOAT3&& rhs) noexcept { m_data = rhs; return *this; }
		inline Vector3& operator=(Vector3&& rhs) noexcept { m_data = rhs.m_data; return *this; }
		inline Vector3& operator=(DirectX::XMVECTOR&& v) noexcept { DirectX::XMStoreFloat3(&m_data, v); return *this; }

		inline operator DirectX::XMFLOAT3() const { return m_data; }
		inline operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(&m_data); }
		inline DirectX::XMVECTOR ToXMVECTOR() const { return DirectX::XMLoadFloat3(&m_data); }

		inline DirectX::XMFLOAT3* operator&() { return &m_data; }
		inline const DirectX::XMFLOAT3* operator&() const { return &m_data; }

		inline float X() const { return m_data.x; }
		inline void X(float x) { m_data.x = x; }

		inline float Y() const { return m_data.y; }
		inline void Y(float y) { m_data.y = y; }

		inline float Z() const { return m_data.z; }
		inline void Z(float z) { m_data.z = z; }

#pragma region Static variable
		static const Vector3 One;
		static const Vector3 Zero;
#pragma endregion

	};
}

namespace AnEngine::DMath
{
	using Vector2 = DirectX::XMFLOAT2;
	using Vector4 = DirectX::XMFLOAT4;

	using SVector = DirectX::XMVECTOR;
	using SVector2 = DirectX::XMVECTOR;
	using SVector3 = DirectX::XMVECTOR;
	using SVector4 = DirectX::XMVECTOR;
	using SVector3th = DirectX::FXMVECTOR;
	using SVector4th = DirectX::GXMVECTOR;
	using SVector56th = DirectX::HXMVECTOR;
	using SVector7oth = DirectX::CXMVECTOR;
}

#endif // !__AENGEIN_VECTOR_H__
