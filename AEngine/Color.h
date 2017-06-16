#pragma once
#ifndef __COLOR_H__
#define __COLOR_H__
#include<DirectXMath.h>
using namespace DirectX;

namespace RenderCore
{
	namespace Resource
	{
		class Color
		{
			XMVECTOR m_color;
		public:
			Color();
			Color(FXMVECTOR vec);
			Color(const XMVECTORF32& vec);
			Color(float r, float g, float b, float a = 1.0f);
			Color(uint16_t r, uint16_t g, uint16_t b, uint16_t a = 255, uint16_t bitDepth = 8);
			explicit Color(uint32_t rgbaLittleEndian);

			~Color() = default;

			float R() const;
			float G() const;
			float B() const;
			float A() const;

			bool operator==(const Color& rhs) const;
			bool operator!=(const Color& rhs) const;

			void SetR(float r);
			void SetG(float g);
			void SetB(float b);
			void SetA(float a);

			float* GetPtr(void);
			float& operator[](int index);

			void SetRGB(float r, float g, float b);

			Color ToSRGB() const;
			Color FromSRGB() const;
			Color ToREC709() const;
			Color FromREC709() const;

			uint32_t R10G10B10A2() const;
			uint32_t R8G8B8A8() const;

			uint32_t R11G11B10F(bool RoundToEven = false) const;

			operator XMVECTOR() const { return m_color; }
		};
	}
}
#endif // !__COLOR_H__
