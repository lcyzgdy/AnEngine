#pragma once
#ifndef __COLORBUFFER_H__
#define __COLORBUFFER_H__

#include"DX.h"
#include"PixelBuffer.h"
using namespace std;
using namespace RenderCore;

namespace RenderCore
{
	namespace Resource
	{
		class ColorBuffer: public PixelBuffer
		{
		protected:

			D3D12_RESOURCE_FLAGS CombineResourceFlags() const;
		public:
			ColorBuffer() = default;
			~ColorBuffer() = default;
		};
	}
}

#endif // !__COLORBUFFER_H__
