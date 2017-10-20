#include "Color.h"

namespace AEngine::RenderCore::Resource
{
	Color::Color() : m_color(g_XMOne)
	{
	}

	Color::Color(FXMVECTOR vec)
	{
		m_color = vec;
	}

	Color::Color(const XMVECTORF32& vec)
	{
		m_color = vec;
	}

	Color::Color(float r, float g, float b, float a)
	{
		m_color = XMVectorSet(r, g, b, a);
	}

	Color::Color(uint16_t r, uint16_t g, uint16_t b, uint16_t a, uint16_t bitDepth)
	{
		m_color = XMVectorScale(XMVectorSet(r, g, b, a), 1.0f / ((1 << bitDepth) - 1));
	}

	Color::Color(uint32_t rgbaLittleEndian)
	{
		float r = (float)((rgbaLittleEndian >> 0) & 0xFF);
		float g = (float)((rgbaLittleEndian >> 8) & 0xFF);
		float b = (float)((rgbaLittleEndian >> 16) & 0xFF);
		float a = (float)((rgbaLittleEndian >> 24) & 0xFF);
		m_color = XMVectorScale(XMVectorSet(r, g, b, a), 1.0f / 255.0f);
	}

	inline float Color::R() const
	{
		return XMVectorGetX(m_color);
	}

	inline float Color::G() const
	{
		return XMVectorGetY(m_color);
	}

	inline float Color::B() const
	{
		return XMVectorGetZ(m_color);
	}

	inline float Color::A() const
	{
		return XMVectorGetW(m_color);
	}

	inline bool Color::operator==(const Color & rhs) const
	{
		return XMVector4Equal(m_color, rhs.m_color);
	}

	inline bool Color::operator!=(const Color & rhs) const
	{
		return !XMVector4Equal(m_color, rhs.m_color);
	}

	inline void Color::SetR(float r)
	{
		m_color = XMVectorSetX(m_color, r);
	}

	inline void Color::SetG(float g)
	{
		m_color = XMVectorSetY(m_color, g);
	}

	inline void Color::SetB(float b)
	{
		m_color = XMVectorSetZ(m_color, b);
	}

	inline void Color::SetA(float a)
	{
		m_color = XMVectorSetW(m_color, a);
	}

	inline float* Color::GetPtr(void)
	{
		return reinterpret_cast<float*>(this);
	}

	inline float& Color::operator[](int index)
	{
		return GetPtr()[index];
	}

	inline void Color::SetRGB(float r, float g, float b)
	{
		m_color = XMVectorSelect(m_color, XMVectorSet(r, g, b, b), g_XMMask3);
	}

	inline Color Color::ToSRGB() const
	{
		XMVECTOR T = XMVectorSaturate(m_color);
		XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(1.0f / 2.4f)), 1.055f), XMVectorReplicate(0.055f));
		result = XMVectorSelect(result, XMVectorScale(T, 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	inline Color Color::FromSRGB() const
	{
		XMVECTOR T = XMVectorSaturate(m_color);
		XMVECTOR result = XMVectorPow(XMVectorScale(XMVectorAdd(T, XMVectorReplicate(0.055f)), 1.0f / 1.055f), XMVectorReplicate(2.4f));
		result = XMVectorSelect(result, XMVectorScale(T, 1.0f / 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	inline Color Color::ToREC709() const
	{
		XMVECTOR T = XMVectorSaturate(m_color);
		XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(0.45f)), 1.099f), XMVectorReplicate(0.099f));
		result = XMVectorSelect(result, XMVectorScale(T, 4.5f), XMVectorLess(T, XMVectorReplicate(0.0018f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	inline Color Color::FromREC709() const
	{
		XMVECTOR T = XMVectorSaturate(m_color);
		XMVECTOR result = XMVectorPow(XMVectorScale(XMVectorAdd(T, XMVectorReplicate(0.099f)), 1.0f / 1.099f), XMVectorReplicate(1.0f / 0.45f));
		result = XMVectorSelect(result, XMVectorScale(T, 1.0f / 4.5f), XMVectorLess(T, XMVectorReplicate(0.0081f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	inline uint32_t Color::R10G10B10A2() const
	{
		XMVECTOR result = XMVectorRound(XMVectorMultiply(XMVectorSaturate(m_color), XMVectorSet(1023.0f, 1023.0f, 1023.0f, 3.0f)));
		result = _mm_castsi128_ps(_mm_cvttps_epi32(result));
		uint32_t r = XMVectorGetIntX(result);
		uint32_t g = XMVectorGetIntY(result);
		uint32_t b = XMVectorGetIntZ(result);
		uint32_t a = XMVectorGetIntW(result) >> 8;
		return a << 30 | b << 20 | g << 10 | r;
	}

	inline uint32_t Color::R8G8B8A8() const
	{
		XMVECTOR result = XMVectorRound(XMVectorMultiply(XMVectorSaturate(m_color), XMVectorReplicate(255.0f)));
		result = _mm_castsi128_ps(_mm_cvttps_epi32(result));
		uint32_t r = XMVectorGetIntX(result);
		uint32_t g = XMVectorGetIntY(result);
		uint32_t b = XMVectorGetIntZ(result);
		uint32_t a = XMVectorGetIntW(result);
		return a << 24 | b << 16 | g << 8 | r;
	}

	inline uint32_t Color::R11G11B10F(bool roundToEven) const
	{
		static XMVECTORU32 Scale = { 0x07800000, 0x07800000, 0x07800000, 0 }; // 2^-112
		static XMVECTORU32 Round1 = { 0x00010000, 0x00010000, 0x00020000, 0 };
		static XMVECTORU32 Round2 = { 0x0000FFFF, 0x0000FFFF, 0x0001FFFF, 0 };
		static XMVECTORU32 Mask = { 0x0FFE0000, 0x0FFE0000, 0x0FFC0000, 0 };

		// Treat the values like integers as we clamp to [0, +Inf].  This translates 32-bit specials
		// to 16-bit specials (while also turning anything greater than MAX_HALF into +INF).
		__m128i ti = _mm_max_epi32(_mm_castps_si128(m_color), _mm_setzero_si128());
		ti = _mm_min_epi32(ti, _mm_set1_epi32(0x47800000)); // 2^16 = 65536.0f = INF

															// Bias the exponent by -112 (-127 + 15) to denormalize values < 2^-14
		ti = _mm_castps_si128(_mm_mul_ps(_mm_castsi128_ps(ti), Scale));

		if (roundToEven)
		{
			// Add 0x10000 when odd, 0x0FFFF when even (before truncating bits)
			ti = _mm_add_epi32(ti, _mm_max_epi32(_mm_and_si128(_mm_srli_epi32(ti, 1), Round1), Round2));
		}
		else //if (RoundToNearest)
		{
			ti = _mm_add_epi32(ti, Round1);
		}

		XMVECTORU32 ret;
		ret.v = _mm_castsi128_ps(_mm_and_si128(ti, Mask));
		return ret.u[0] >> 17 | ret.u[1] >> 6 | ret.u[2] << 4;
	}

	inline Color::operator XMVECTOR() const
	{
		return m_color;
	}
}