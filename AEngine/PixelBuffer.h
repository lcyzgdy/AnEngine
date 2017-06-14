#pragma once
#ifndef __PIXELBUFFER_H__
#define __PIXELBUFFER_H__
#include"DX.h"
#include"GpuResource.h"
using namespace std;
using namespace RenderCore::Resource;

namespace RenderCore
{
	namespace Resource
	{
		class PixelBuffer :public GpuResource
		{
		protected:
			UINT m_width;
			UINT m_height;
			UINT m_size;
			UINT m_bankRotation;
			DXGI_FORMAT m_format;
		public:
			PixelBuffer();
			~PixelBuffer() = default;

			DXGI_FORMAT GetBaseFormat(DXGI_FORMAT& format);
			DXGI_FORMAT GetUAVFormat(DXGI_FORMAT& format);
			DXGI_FORMAT GetDSBFormat(DXGI_FORMAT& format);
			DXGI_FORMAT GetDepthFormat(DXGI_FORMAT& format);
		};
	}
}

#endif // !__PIXELBUFFER_H__
