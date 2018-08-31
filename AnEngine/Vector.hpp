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
	/*template<size_t n>
	struct Vector
	{
	public:
		Vector()
		{
			if (n > 4) throw std::exception("Too many number for a sse vector.");
			if (n <= 0) throw std::exception("Too few number for a sse vector.");
		}
	};

	struct Vector2 :Vector<2>
	{
		DirectX::XMFLOAT2 m_vector;
	public:
		Vector2() = default;
		Vector2(const Vector2& v) :m_vector(v.m_vector)
		{
		}
		Vector2(float x, float y) :m_vector(x, y)
		{
		}
		Vector2(const DirectX::XMFLOAT2& f2) :m_vector(f2)
		{
		}

		///////////////////////////////////////////////////////////////////////////////////////

		inline float X()
		{
			return m_vector.x;
		}

		inline void X(float x)
		{
			m_vector.x = x;
		}

		inline float Y()
		{
			return m_vector.y;
		}

		inline void Y(float y)
		{
			m_vector.y = y;
		}

		inline static Vector2&& Cross(const Vector2& v1, const Vector2& v2)
		{
			Vector2 temp;
			DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&v1.m_vector), DirectX::XMLoadFloat2(&v2.m_vector)));
			return std::move(temp);
		}

		inline float Dot(const Vector2& v1, const Vector2& v2)
		{
			return DirectX::XMVectorGetX(DirectX::XMVector2Dot(DirectX::XMLoadFloat2(&v1.m_vector), DirectX::XMLoadFloat2(&v2.m_vector)));
		}

		inline static Vector2&& Add(const Vector2& v1, const Vector2& v2)
		{
			Vector2 temp;
			DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&v1.m_vector), DirectX::XMLoadFloat2(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector2&& Subtract(const Vector2& v1, const Vector2& v2)
		{
			Vector2 temp;
			DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&v1.m_vector), DirectX::XMLoadFloat2(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector2&& Multiply(const Vector2& v, const float n)
		{
			Vector2 temp;
			DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&v.m_vector), n));
			return std::move(temp);
		}

		/////////////////////////////////////////////

		inline Vector2& operator+=(const Vector2& v)
		{
			DirectX::XMStoreFloat2(&m_vector, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&m_vector), DirectX::XMLoadFloat2(&v.m_vector)));
			return *this;
		}

		inline Vector2& operator-=(const Vector2& v)
		{
			DirectX::XMStoreFloat2(&m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&m_vector), DirectX::XMLoadFloat2(&v.m_vector)));
			return *this;
		}

		inline Vector2& operator*=(const Vector2& v)
		{
			DirectX::XMStoreFloat2(&m_vector, DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&m_vector), DirectX::XMLoadFloat2(&v.m_vector)));
			return *this;
		}

		inline Vector2& operator*=(float n)
		{
			DirectX::XMStoreFloat2(&m_vector, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&m_vector), n));
			return *this;
		}

		inline Vector2& operator=(const Vector2& v)
		{
			Vector2 temp(v);
			return std::move(temp);
		}

		///////////////////////////////////////////////////////////////////////////////////

		friend inline Vector2&& operator*(const Vector2& v1, const Vector2& v2);
		friend inline Vector2&& operator+(const Vector2& v1, const Vector2& v2);
		friend inline Vector2&& operator-(const Vector2& v1, const Vector2& v2);
		friend inline Vector2&& operator*(const Vector2& v1, const float n);
		friend inline Vector2&& operator*(const float n, const Vector2& v1);

		///////////////////////////////////////////////////////////////////////////////////

		inline static Vector2&& Normalize(Vector2& v)
		{
			Vector2 temp;
			DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&v.m_vector)));
			return std::move(temp);
		}

		inline void Normalize()
		{
			DirectX::XMStoreFloat2(&m_vector, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&m_vector)));
		}
	};


	Vector2&& operator*(const Vector2& v1, const Vector2& v2)
	{
		Vector2 temp;
		DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&v1.m_vector), DirectX::XMLoadFloat2(&v2.m_vector)));
		return std::move(temp);
	}

	Vector2&& operator+(const Vector2& v1, const Vector2& v2)
	{
		Vector2 temp;
		DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVectorAdd(XMLoadFloat2(&v1.m_vector), DirectX::XMLoadFloat2(&v2.m_vector)));
		return std::move(temp);
	}

	Vector2&& operator-(const Vector2& v1, const Vector2& v2)
	{
		Vector2 temp;
		DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&v1.m_vector), DirectX::XMLoadFloat2(&v2.m_vector)));
		return std::move(temp);
	}

	Vector2&& operator*(const Vector2& v1, const float n)
	{
		Vector2 temp;
		DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&v1.m_vector), n));
		return std::move(temp);
	}

	Vector2&& operator*(const float n, const Vector2& v1)
	{
		Vector2 temp;
		DirectX::XMStoreFloat2(&temp.m_vector, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&v1.m_vector), n));
		return std::move(temp);
	}
}
namespace AnEngine::DMath
{
	struct Vector3 :Vector<3>
	{
		DirectX::XMFLOAT3 m_vector;
	public:
		Vector3() = default;
		Vector3(const Vector3& v) :m_vector(v.m_vector)
		{
		}
		Vector3(float x, float y, float z) :m_vector(x, y, z)
		{
		}
		Vector3(DirectX::XMFLOAT3&& f2) :m_vector(f2)
		{
		}
		Vector3(float x) :m_vector(x, x, x)
		{
		}

		///////////////////////////////////////////////////////////////////////////////////////

		inline float X()
		{
			return m_vector.x;
		}

		inline void X(float x)
		{
			m_vector.x = x;
		}

		inline float Y()
		{
			return m_vector.y;
		}

		inline void Y(float y)
		{
			m_vector.y = y;
		}

		inline float Z()
		{
			return m_vector.z;
		}

		inline void Z(float z)
		{
			m_vector.z = z;
		}



		//////////////////////////////////////////////////////////////////////////////////////////////
		//																							//
		//																							//
		//																							//
		//////////////////////////////////////////////////////////////////////////////////////////////

		inline static Vector3&& Cross(const Vector3& v1, const Vector3& v2)
		{
			Vector3 temp;
			DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&v1.m_vector), DirectX::XMLoadFloat3(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector3&& Dot(const Vector3& v1, const Vector3& v2)
		{
			Vector3 temp;
			DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&v1.m_vector), DirectX::XMLoadFloat3(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector3&& Add(const Vector3& v1, const Vector3& v2)
		{
			Vector3 temp;
			DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&v1.m_vector), DirectX::XMLoadFloat3(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector3&& Subtract(const Vector3& v1, const Vector3& v2)
		{
			Vector3 temp;
			DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&v1.m_vector), DirectX::XMLoadFloat3(&v2.m_vector)));
			return std::move(temp);
		}

		inline Vector3& operator+=(const Vector3& v)
		{
			DirectX::XMStoreFloat3(&m_vector, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_vector), DirectX::XMLoadFloat3(&v.m_vector)));
			return *this;
		}

		inline Vector3& operator-=(const Vector3& v)
		{
			DirectX::XMStoreFloat3(&m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_vector), DirectX::XMLoadFloat3(&v.m_vector)));
			return *this;
		}

		inline Vector3& operator*=(const Vector3& v)
		{
			DirectX::XMStoreFloat3(&m_vector, DirectX::XMVector2Cross(DirectX::XMLoadFloat3(&m_vector), DirectX::XMLoadFloat3(&v.m_vector)));
			return *this;
		}

		inline Vector3& operator*=(float n)
		{
			DirectX::XMStoreFloat3(&m_vector, DirectX::XMVectorScale(XMLoadFloat3(&m_vector), n));
			return *this;
		}

		inline Vector3& operator=(const Vector3& v)
		{
			DirectX::XMStoreFloat3(&m_vector, DirectX::XMLoadFloat3(&v.m_vector));
			return *this;
		}


		friend inline Vector3&& operator*(const Vector3& v1, const Vector3& v2);
		friend inline Vector3&& operator+(const Vector3& v1, const Vector3& v2);
		friend inline Vector3&& operator-(const Vector3& v1, const Vector3& v2);

		///////////////////////////////////////////////////////////////////////////////////

		inline static Vector3&& Normalize(const Vector3& v)
		{
			Vector3 temp;
			DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVector2Normalize(DirectX::XMLoadFloat3(&v.m_vector)));
			return std::move(temp);
		}

		inline void Normalize()
		{
			DirectX::XMStoreFloat3(&m_vector, DirectX::XMVector2Normalize(DirectX::XMLoadFloat3(&m_vector)));
		}
	};

	Vector3&& operator*(const Vector3& v1, const Vector3& v2)
	{
		Vector3 temp;
		DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&v1.m_vector), DirectX::XMLoadFloat3(&v2.m_vector)));
		return std::move(temp);
	}

	Vector3&& operator+(const Vector3& v1, const Vector3& v2)
	{
		Vector3 temp;
		DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVectorAdd(XMLoadFloat3(&v1.m_vector), DirectX::XMLoadFloat3(&v2.m_vector)));
		return std::move(temp);
	}

	Vector3&& operator-(const Vector3& v1, const Vector3& v2)
	{
		Vector3 temp;
		DirectX::XMStoreFloat3(&temp.m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&v1.m_vector), DirectX::XMLoadFloat3(&v2.m_vector)));
		return std::move(temp);
	}
}

namespace AnEngine::DMath
{
	struct Vector4 :Vector<4>
	{
		DirectX::XMFLOAT4 m_vector;

	public:
		Vector4() = default;
		Vector4(const Vector4& v) :m_vector(v.m_vector) {}
		Vector4(float x) :m_vector(x, 0, 0, 0) {}
		Vector4(float x, float y) :m_vector(x, y, 0, 0) {}
		Vector4(float x, float y, float z) :m_vector(x, y, z, 0) {}
		Vector4(float x, float y, float z, float w) :m_vector(x, y, z, w) {}
		Vector4(DirectX::XMFLOAT4&& f2) :m_vector(f2) {}

		///////////////////////////////////////////////////////////////////////////////////////

		inline float X()
		{
			return m_vector.x;
		}

		inline void X(float x)
		{
			m_vector.x = x;
		}

		inline float Y()
		{
			return m_vector.y;
		}

		inline void Y(float y)
		{
			m_vector.y = y;
		}

		inline float Z()
		{
			return m_vector.z;
		}

		inline void Z(float z)
		{
			m_vector.z = z;
		}

		inline float W()
		{
			return m_vector.w;
		}

		inline void W(float w)
		{
			m_vector.w = w;
		}

		inline static Vector4&& Cross(const Vector4& v1, const Vector4& v2)
		{
			Vector4 temp;
			DirectX::XMStoreFloat4(&temp.m_vector, DirectX::XMVector3Cross(DirectX::XMLoadFloat4(&v1.m_vector), DirectX::XMLoadFloat4(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector4&& Dot(const Vector4& v1, const Vector4& v2)
		{
			Vector4 temp;
			DirectX::XMStoreFloat4(&temp.m_vector, DirectX::XMVector4Dot(DirectX::XMLoadFloat4(&v1.m_vector), DirectX::XMLoadFloat4(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector4&& Add(const Vector4& v1, const Vector4& v2)
		{
			Vector4 temp;
			DirectX::XMStoreFloat4(&temp.m_vector, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&v1.m_vector), DirectX::XMLoadFloat4(&v2.m_vector)));
			return std::move(temp);
		}

		inline static Vector4&& Subtract(const Vector4& v1, const Vector4& v2)
		{
			Vector4 temp;
			DirectX::XMStoreFloat4(&temp.m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&v1.m_vector), DirectX::XMLoadFloat4(&v2.m_vector)));
			return std::move(temp);
		}

		inline Vector4& operator+=(const Vector4& v)
		{
			DirectX::XMStoreFloat4(&m_vector, DirectX::XMVectorAdd(XMLoadFloat4(&m_vector), DirectX::XMLoadFloat4(&v.m_vector)));
			return *this;
		}

		inline Vector4& operator-=(const Vector4& v)
		{
			DirectX::XMStoreFloat4(&m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&m_vector), DirectX::XMLoadFloat4(&v.m_vector)));
			return *this;
		}

		inline Vector4& operator*=(const Vector4& v)
		{
			DirectX::XMStoreFloat4(&m_vector, DirectX::XMVector2Cross(DirectX::XMLoadFloat4(&m_vector), DirectX::XMLoadFloat4(&v.m_vector)));
			return *this;
		}

		inline Vector4& operator*=(float n)
		{
			DirectX::XMStoreFloat4(&m_vector, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&m_vector), n));
			return *this;
		}

		inline Vector4& operator=(const Vector4& v)
		{
			DirectX::XMStoreFloat4(&m_vector, DirectX::XMLoadFloat4(&v.m_vector));
			return *this;
		}

		friend inline Vector4&& operator+(const Vector4& v1, const Vector4& v2);
		friend inline Vector4&& operator-(const Vector4& v1, const Vector4& v2);

		///////////////////////////////////////////////////////////////////////////////////

		inline static Vector4&& Normalize(const Vector4& v)
		{
			Vector4 temp;
			DirectX::XMStoreFloat4(&temp.m_vector, DirectX::XMVector2Normalize(DirectX::XMLoadFloat4(&v.m_vector)));
			return std::move(temp);
		}

		inline void Normalize()
		{
			DirectX::XMStoreFloat4(&m_vector, DirectX::XMVector2Normalize(DirectX::XMLoadFloat4(&m_vector)));
		}
	};

	Vector4&& operator+(const Vector4& v1, const Vector4& v2)
	{
		Vector4 temp;
		DirectX::XMStoreFloat4(&temp.m_vector, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&v1.m_vector), DirectX::XMLoadFloat4(&v2.m_vector)));
		return std::move(temp);
	}

	Vector4&& operator-(const Vector4& v1, const Vector4& v2)
	{
		Vector4 temp;
		DirectX::XMStoreFloat4(&temp.m_vector, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&v1.m_vector), DirectX::XMLoadFloat4(&v2.m_vector)));
		return std::move(temp);
	}*/

	using Vector2 = DirectX::XMFLOAT2;
	using Vector3 = DirectX::XMFLOAT3;
	using Vector4 = DirectX::XMFLOAT4;

	using SVector = DirectX::XMVECTOR;
	using SVector3th = DirectX::FXMVECTOR;
	using SVector4th = DirectX::GXMVECTOR;
	using SVector56th = DirectX::HXMVECTOR;
	using SVector7oth = DirectX::CXMVECTOR;
}

#endif // !__AENGEIN_VECTOR_H__
