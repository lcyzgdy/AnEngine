#include "Color.h"

namespace AnEngine::RenderCore::Resource
{
	Color::Color() : m_color(g_XMOne)
	{
	}

	Color::Color(const Color & color) : m_color(color.m_color)
	{
	}

	Color::Color(XMVECTOR vec)
	{
		//= vec;
		XMStoreFloat4(&m_color, vec);
	}

	Color::Color(const XMVECTORF32& vec)
	{
		//m_color = vec;
		XMStoreFloat4(&m_color, vec);
	}

	Color::Color(float r, float g, float b, float a)
	{
		//m_color = XMVectorSet(r, g, b, a);
		//m_color = XMFLOAT4(r, g, b, a);
		//XMStoreFloat4(&m_color, XMVectorSet(r, g, b, a));
		m_color = { r, g, b, a };
	}

	Color::Color(uint32_t rgbaLittleEndian)
	{
		float r = (float)((rgbaLittleEndian >> 0) & 0xFF);
		float g = (float)((rgbaLittleEndian >> 8) & 0xFF);
		float b = (float)((rgbaLittleEndian >> 16) & 0xFF);
		float a = (float)((rgbaLittleEndian >> 24) & 0xFF);
		//m_color = XMVectorScale(XMVectorSet(r, g, b, a), 1.0f / 255.0f);
		XMStoreFloat4(&m_color, XMVectorScale(XMVectorSet(r, g, b, a), 1.0f / 255.0f));
	}

	float Color::GetR() const
	{
		//return XMVectorGetX(m_color);
		return m_color.x;
	}

	float Color::GetG() const
	{
		//return XMVectorGetY(m_color);
		return m_color.y;
	}

	float Color::GetB() const
	{
		//return XMVectorGetZ(m_color);
		return m_color.z;
	}

	float Color::GetA() const
	{
		//return XMVectorGetW(m_color);
		return m_color.w;
	}

	bool Color::operator==(const Color & rhs) const
	{
		//return XMVector4Equal(m_color, rhs.m_color);
		return m_color.x == rhs.m_color.x && m_color.y == rhs.m_color.y
			&& m_color.z == rhs.m_color.z && m_color.w == rhs.m_color.w;
	}

	bool Color::operator!=(const Color & rhs) const
	{
		//return !XMVector4Equal(m_color, rhs.m_color);
		return !(m_color.x == rhs.m_color.x && m_color.y == rhs.m_color.y
			&& m_color.z == rhs.m_color.z && m_color.w == rhs.m_color.w);
	}

	void Color::SetR(float r)
	{
		//m_color = XMVectorSetX(m_color, r);
		m_color.x = r;
	}

	void Color::SetG(float g)
	{
		//m_color = XMVectorSetY(m_color, g);
		m_color.y = g;
	}

	void Color::SetB(float b)
	{
		//m_color = XMVectorSetZ(m_color, b);
		m_color.z = b;
	}

	void Color::SetA(float a)
	{
		//m_color = XMVectorSetW(m_color, a);
		m_color.w = a;
	}

	float* Color::GetPtr(void)
	{
		return reinterpret_cast<float*>(&m_color);
	}

	/*inline float& Color::operator[](int index)
	{
		return GetPtr()[index];
	}*/

	void Color::SetRGB(float r, float g, float b)
	{
		//m_color = XMVectorSelect(m_color, XMVectorSet(r, g, b, b), g_XMMask3);
		m_color.x = r;
		m_color.y = g;
		m_color.z = b;
	}

	Color Color::ToSRGB() const
	{
		XMVECTOR T = XMVectorSaturate(XMLoadFloat4(&m_color));
		XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(1.0f / 2.4f)), 1.055f), XMVectorReplicate(0.055f));
		result = XMVectorSelect(result, XMVectorScale(T, 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	Color Color::FromSRGB() const
	{
		//XMVECTOR T = XMVectorSaturate(m_color);
		XMVECTOR T = XMVectorSaturate(XMLoadFloat4(&m_color));
		XMVECTOR result = XMVectorPow(XMVectorScale(XMVectorAdd(T, XMVectorReplicate(0.055f)), 1.0f / 1.055f), XMVectorReplicate(2.4f));
		result = XMVectorSelect(result, XMVectorScale(T, 1.0f / 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	Color Color::ToREC709() const
	{
		//XMVECTOR T = XMVectorSaturate(m_color);
		XMVECTOR T = XMVectorSaturate(XMLoadFloat4(&m_color));
		XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(0.45f)), 1.099f), XMVectorReplicate(0.099f));
		result = XMVectorSelect(result, XMVectorScale(T, 4.5f), XMVectorLess(T, XMVectorReplicate(0.0018f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	Color Color::FromREC709() const
	{
		//XMVECTOR T = XMVectorSaturate(m_color);
		XMVECTOR T = XMVectorSaturate(XMLoadFloat4(&m_color));
		XMVECTOR result = XMVectorPow(XMVectorScale(XMVectorAdd(T, XMVectorReplicate(0.099f)), 1.0f / 1.099f), XMVectorReplicate(1.0f / 0.45f));
		result = XMVectorSelect(result, XMVectorScale(T, 1.0f / 4.5f), XMVectorLess(T, XMVectorReplicate(0.0081f)));
		return XMVectorSelect(T, result, g_XMSelect1110);
	}

	uint32_t Color::R10G10B10A2() const
	{
		XMVECTOR result = XMVectorRound(XMVectorMultiply(XMVectorSaturate(XMLoadFloat4(&m_color)), XMVectorSet(1023.0f, 1023.0f, 1023.0f, 3.0f)));
		result = _mm_castsi128_ps(_mm_cvttps_epi32(result));
		uint32_t r = XMVectorGetIntX(result);
		uint32_t g = XMVectorGetIntY(result);
		uint32_t b = XMVectorGetIntZ(result);
		uint32_t a = XMVectorGetIntW(result) >> 8;
		return a << 30 | b << 20 | g << 10 | r;
	}

	uint32_t Color::R8G8B8A8() const
	{
		XMVECTOR result = XMVectorRound(XMVectorMultiply(XMVectorSaturate(XMLoadFloat4(&m_color)), XMVectorReplicate(255.0f)));
		result = _mm_castsi128_ps(_mm_cvttps_epi32(result));
		uint32_t r = XMVectorGetIntX(result);
		uint32_t g = XMVectorGetIntY(result);
		uint32_t b = XMVectorGetIntZ(result);
		uint32_t a = XMVectorGetIntW(result);
		return a << 24 | b << 16 | g << 8 | r;
	}

	uint32_t Color::R11G11B10F(bool roundToEven) const
	{
		static XMVECTORU32 Scale = { 0x07800000, 0x07800000, 0x07800000, 0 }; // 2^-112
		static XMVECTORU32 Round1 = { 0x00010000, 0x00010000, 0x00020000, 0 };
		static XMVECTORU32 Round2 = { 0x0000FFFF, 0x0000FFFF, 0x0001FFFF, 0 };
		static XMVECTORU32 Mask = { 0x0FFE0000, 0x0FFE0000, 0x0FFC0000, 0 };

		// Treat the values like integers as we clamp to [0, +Inf].  This translates 32-bit specials
		// to 16-bit specials (while also turning anything greater than MAX_HALF into +INF).
		__m128i ti = _mm_max_epi32(_mm_castps_si128(XMLoadFloat4(&m_color)), _mm_setzero_si128());
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

	D2D1::ColorF Color::ToD2DColor() const
	{
		return D2D1::ColorF(m_color.x, m_color.y, m_color.z, m_color.w);
	}

	/*inline Color::operator XMVECTOR() const
	{
		return XMLoadFloat4(&m_color);
	}*/

	const Color Color::Black = Color(0, 0, 0, 1);
	const Color Color::Blue = Color(0, 0, 1, 1);
	const Color Color::Green = Color(0, 1, 0, 1);
	const Color Color::Red = Color(1, 0, 0, 1);
	const Color Color::White = Color(1, 1, 1, 1);



	Color32::Color32(XMVECTOR vec)
	{
		XMStoreInt4(reinterpret_cast<uint32_t*>(&m_color), vec);
	}

	Color32::Color32(const XMVECTORF32 & vec)
	{
	}

	Color32::Color32(int r, int g, int b, int a)
	{
	}

	Color32::Color32(uint32_t rgbaLittleEndian)
	{
	}

	int Color32::R() const
	{
		return m_color.x;
	}

	int Color32::G() const
	{
		return m_color.y;
	}

	int Color32::B() const
	{
		return m_color.z;
	}

	int Color32::A() const
	{
		return m_color.w;
	}

	bool Color32::operator==(const Color32 & rhs) const
	{
		return m_color.x == rhs.m_color.x && m_color.y == rhs.m_color.y
			&& m_color.z == rhs.m_color.z && m_color.w == rhs.m_color.w;
	}

	bool Color32::operator!=(const Color32 & rhs) const
	{
		return !(m_color.x == rhs.m_color.x && m_color.y == rhs.m_color.y
			&& m_color.z == rhs.m_color.z && m_color.w == rhs.m_color.w);
	}

	void Color32::SetR(int r)
	{
		if (r > 255) r = 255;
		if (r < 0) r = 0;
		m_color.x = r;
	}

	void Color32::SetG(int g)
	{
		if (g > 255) g = 255;
		if (g < 0) g = 0;
		m_color.y = g;
	}

	void Color32::SetB(int b)
	{
		if (b > 255) b = 255;
		if (b < 0) b = 0;
		m_color.z = b;
	}

	void Color32::SetA(int a)
	{
		if (a > 255) a = 255;
		if (a < 0) a = 0;
		m_color.w = a;
	}

	int * Color32::GetPtr(void)
	{
		return reinterpret_cast<int*>(&m_color);
	}

	void Color32::SetRGB(int r, int g, int b)
	{
		m_color.x = r;
		m_color.y = g;
		m_color.z = b;
	}

	Color32 Color32::ToSRGB() const
	{
		return Color32();
	}

	Color32 Color32::FromSRGB(Color32 & srgb) const
	{
		return Color32();
	}

	Color32 Color32::ToREC709() const
	{
		return Color32();
	}

	Color32 Color32::FromREC709(Color32 & srgb) const
	{
		return Color32();
	}

	uint32_t Color32::R10G10B10A2() const
	{
		return uint32_t();
	}

	uint32_t Color32::R8G8B8A8() const
	{
		return uint32_t();
	}

	uint32_t Color32::R11G11B10F(bool RoundToEven) const
	{
		return uint32_t();
	}
}