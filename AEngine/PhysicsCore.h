#pragma once
#ifndef __PHYSICSCORE_H__
#define __PHYSICSCORE_H__

#include"onwind.h"
#include<DirectXMath.h>
using namespace std;
using namespace DirectX;

namespace PhysicsCore
{
	class Transform
	{
		XMVECTOR position;
		XMVECTOR rotation;
		XMVECTOR scale;
	public:
		Transform() = default;
		~Transform() = default;

		XMVECTOR GetPosition()
		{
			return position;
		}

		void SetPosition(XMFLOAT3& newPosition)
		{
			position = { newPosition.x, newPosition.y, newPosition.z, 0 };
		}

		void SetPosition(XMVECTOR& newPosition)
		{
			position = newPosition;
		}

		float X()
		{
			return position.m128_f32[0];
		}

		float Y()
		{
			return position.m128_f32[1];
		}
		float Z()
		{
			return position.m128_f32[2];
		}
	};
}

#endif // __PHYSICSCORE_H__
