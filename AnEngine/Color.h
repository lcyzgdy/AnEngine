#pragma once
#ifndef __COLOR_H__
#define __COLOR_H__
#include <DirectXMath.h>
#include <d2d1_3.h>
using namespace DirectX;

namespace AnEngine::RenderCore::Resource
{
	class Color
	{
		//XMVECTOR m_color;
		XMFLOAT4 m_color;

		Color(XMVECTOR vec);
		Color(const XMVECTORF32& vec);

	public:
		Color();
		Color(const Color& color);
		Color(float r, float g, float b, float a = 1.0f);
		//Color(uint16_t r, uint16_t g, uint16_t b, uint16_t a = 255, uint16_t bitDepth = 8);
		explicit Color(uint32_t rgbaLittleEndian);

		~Color() = default;

		float GetR() const;
		float GetG() const;
		float GetB() const;
		float GetA() const;

		//void R(float r) const;
		//void G(float g) const;
		//void B(float b) const;
		//void A(float a) const;

		bool operator==(const Color& rhs) const;
		bool operator!=(const Color& rhs) const;

		void SetR(float r);
		void SetG(float g);
		void SetB(float b);
		void SetA(float a);

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
		//XMVECTOR m_color;
		XMINT4 m_color;
	public:
		Color32() = default;
		Color32(XMVECTOR vec);
		Color32(const XMVECTORF32& vec);
		Color32(int r, int g, int b, int a = 255);
		//Color(uint16_t r, uint16_t g, uint16_t b, uint16_t a = 255, uint16_t bitDepth = 8);
		explicit Color32(uint32_t rgbaLittleEndian);

		~Color32() = default;

		int R() const;
		int G() const;
		int B() const;
		int A() const;

		bool operator==(const Color32& rhs) const;
		bool operator!=(const Color32& rhs) const;

		void SetR(int r);
		void SetG(int g);
		void SetB(int b);
		void SetA(int a);

		int* GetPtr(void);
		//float& operator[](int index);

		void SetRGB(int r, int g, int b);

		Color32 ToSRGB() const;
		Color32 FromSRGB(Color32& srgb) const;
		Color32 ToREC709() const;
		Color32 FromREC709(Color32& srgb) const;

		uint32_t R10G10B10A2() const;
		uint32_t R8G8B8A8() const;

		uint32_t R11G11B10F(bool RoundToEven = false) const;

		//operator XMVECTOR() const;
	};
}

#endif // !__COLOR_H__
