#pragma once
#ifndef __PHYSICSCORE_H__
#define __PHYSICSCORE_H__

#include"onwind.h"
#include<DirectXMath.h>
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
	};
}

#endif // __PHYSICSCORE_H__
