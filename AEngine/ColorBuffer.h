#pragma once
#ifndef __COLORBUFFER_H__
#define __COLORBUFFER_H__

#include"DX.h"
#include"PixelBuffer.h"
using namespace std;

namespace RenderCore
{
	namespace Resource
	{
		class ColorBuffer
		{
		public:
			ColorBuffer() = default;
			~ColorBuffer() = default;
		};
	}
}

#endif // !__COLORBUFFER_H__
