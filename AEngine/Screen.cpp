#include "Screen.h"

void Screen::Initialize(const int _width, const int _height)
{
	m_width = _width;
	m_height = _height;
}

inline Screen * Screen::GetInstance()
{
	static Screen screen;
	return &screen;
}
