#include "PixelBuffer.h"

RenderCore::Resource::PixelBuffer::PixelBuffer() :
	m_width(0), m_height(0), m_format(DXGI_FORMAT_UNKNOWN), m_bankRotation(0),
	m_size(0)
{
}
