﻿#pragma once
#ifndef __COLOR_H__
#define __COLOR_H__
#include <DirectXMath.h>
#include <d2d1_3.h>
#include "onwind.h"

namespace AnEngine
{
	class DLL_API Color
	{
		DirectX::XMFLOAT4 m_color;
		bool m_hdr;
		bool m_linear;		// 是否为线性空间
		float m_hdrIntensity;

		Color(DirectX::XMVECTOR vec);
		Color(const DirectX::XMVECTORF32& vec);

	public:
		Color();
		Color(const Color& color);
		Color(float r, float g, float b, float a = 1.0f);
		Color(bool enableHdr, float hdrIntensity, float r, float g, float b, float a = 1.0f);
		explicit Color(uint32_t rgbaLittleEndian);

		~Color() = default;

		inline const float GetR() const { return m_color.x; }
		inline const float GetG() const { return m_color.y; };
		inline const float GetB() const { return m_color.z; };
		inline const float GetA() const { return m_color.w; };

		//void R(float r) const;
		//void G(float g) const;
		//void B(float b) const;
		//void A(float a) const;

		bool operator==(const Color& rhs) const;
		bool operator!=(const Color& rhs) const;

		inline void SetR(float r) { m_color.x = r; }
		inline void SetG(float g) { m_color.y = g; };
		inline void SetB(float b) { m_color.z = b; };
		inline void SetA(float a) { m_color.w = a; };

		float* GetPtr(void);
		//float& operator[](int index);

		void SetRGB(float r, float g, float b);

		Color ToSRGB() const;
		Color FromSRGB() const;
		Color ToREC709() const;
		Color FromREC709() const;

		uint32_t R10G10B10A2() const;
		uint32_t R8G8B8A8() const;

		uint32_t R11G11B10F(bool RoundToEven = false) const;

		D2D1::ColorF ToD2DColor() const;

		//operator XMVECTOR() const;

		static const Color Black;
		static const Color White;
		static const Color Red;
		static const Color Blue;
		static const Color Green;
	};

	class Color32
	{
		DirectX::XMINT4 m_color;
	public:
		Color32() = default;
		Color32(DirectX::XMVECTOR vec);
		Color32(const DirectX::XMVECTORF32& vec);
		Color32(int r, int g, int b, int a = 255);
		explicit Color32(uint32_t rgbaLittleEndian);

		~Color32() = default;

		inline const int R() const { return m_color.x; }
		inline const int G() const { return m_color.y; }
		inline const int B() const { return m_color.z; }
		inline const int A() const { return m_color.w; }

		bool operator==(const Color32& rhs) const;
		bool operator!=(const Color32& rhs) const;

		void SetR(int r);
		void SetG(int g);
		void SetB(int b);
		void SetA(int a);

		int* GetPtr(void);

		void SetRGB(int r, int g, int b);

		Color32 ToSRGB() const;
		Color32 FromSRGB(Color32& srgb) const;
		Color32 ToREC709() const;
		Color32 FromREC709(Color32& srgb) const;

		uint32_t R10G10B10A2() const;
		uint32_t R8G8B8A8() const;

		uint32_t R11G11B10F(bool RoundToEven = false) const;
	};
}

#endif // !__COLOR_H__
