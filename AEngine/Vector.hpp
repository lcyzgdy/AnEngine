#pragma once
#ifndef __AENGEIN_VECTOR_HPP__
#define __AENGINE_VECTOR_HPP__

#include"../Utility/onwind.h"
#include<DirectXMath.h>
using namespace DirectX;

/*(http://blog.csdn.net/zjjzhaohang/article/details/68936082)
为了效率起见，在DX12中传参XMVECTOR 时，有一定规则的调用方式，以便利用SSE/SSE2硬件加速。
1、前三个XMVECTOR 参数调用时，应该使用FXMVECTOR
2、第四个XMVECTOR参数，应该使用GXMVECTOR
3、第五、六个XMVECTOR参数，应该使用HXMVECTOR
4、剩下的额外的参数使用CXMVECTOR
5、若其中穿插了其他类型参数，规则忽略其他参数照样生效
*/

namespace Math
{
	template<size_t n>
	class Vector
	{
		XMFLOAT2 a;
	public:
		Vector()
		{
			if (n > 4) throw std::exception("Too many number for a sse vector.");
			if (n <= 0) throw std::exception("Too few number for a sse vector.")
		}

		static inline XMVECTOR __vectorcall Cross(XMVECTOR v1, XMVECTOR v2)
		{
		}
	};

	class Vector2 :Vector<2>
	{
		XMFLOAT2 m_vector;
	public:
		Vector2() = default;
		Vector2(Vector2&& v) :m_vector(v.m_vector)
		{
		}
	};

	class Vector3 :Vector<3>
	{
		XMFLOAT3 m_vector;
	public:
		Vector3() = default;
		Vector3(Vector3&& v) :m_vector(v.m_vector)
		{
		}
	};


}

#endif // !__AENGEIN_VECTOR_H__
