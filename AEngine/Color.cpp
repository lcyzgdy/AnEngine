#include "Color.h"

namespace RenderCore
{
	namespace Resource
	{
		Color::Color() : m_color(g_XMOne)
		{
		}

		Color::Color(FXMVECTOR vec)
		{
		}

		Color::Color(const XMVECTORF32 & vec)
		{
		}

		Color::Color(float r, float g, float b, float a)
		{
		}

		Color::Color(uint16_t r, uint16_t g, uint16_t b, uint16_t a, uint16_t bitDepth)
		{
		}

		Color::Color(uint32_t rgbaLittleEndian)
		{
		}

		float Color::R() const
		{
			return XMVectorGetX(m_color);
		}

		float Color::G() const
		{
			return XMVectorGetY(m_color);
		}

		float Color::B() const
		{
			return XMVectorGetZ(m_color);
		}

		float Color::A() const
		{
			return XMVectorGetW(m_color);
		}

		bool Color::operator==(const Color & rhs) const
		{
			return XMVector4Equal(m_color, rhs.m_color);
		}

		bool Color::operator!=(const Color & rhs) const
		{
			return !XMVector4Equal(m_color, rhs.m_color);
		}

		void Color::SetR(float r)
		{
			m_color = XMVectorSetX(m_color, r);
		}

		void Color::SetG(float g)
		{
			m_color = XMVectorSetY(m_color, g);
		}

		void Color::SetB(float b)
		{
			m_color = XMVectorSetZ(m_color, b);
		}

		void Color::SetA(float a)
		{
			m_color = XMVectorSetW(m_color, a);
		}

		float * Color::GetPtr(void)
		{
			return reinterpret_cast<float*>(this);
		}

		float & Color::operator[](int index)
		{
			return GetPtr()[index];
		}

		void Color::SetRGB(float r, float g, float b)
		{
			m_color = XMVectorSelect(m_color, XMVectorSet(r, g, b, b), g_XMMask3);
		}
		Color Color::ToSRGB() const
		{
			return Color();
		}
		Color Color::FromSRGB() const
		{
			return Color();
		}
		Color Color::ToREC709() const
		{
			return Color();
		}
		Color Color::FromREC709() const
		{
			return Color();
		}
		uint32_t Color::R10G10B10A2() const
		{
			return uint32_t();
		}
		uint32_t Color::R8G8B8A8() const
		{
			return uint32_t();
		}
		uint32_t Color::R11G11B10F(bool RoundToEven) const
		{
			return uint32_t();
		}
		Color::operator XMVECTOR() const
		{
			return m_color;
		}
	}
}