#include "Screen.h"

namespace AnEngine
{
	DECLEAR_UNIQUE_OBJ(Screen);
	DECLEAR_INSTANCE(Screen);

	void Screen::Initialize(const int _width, const int _height)
	{
		m_width = _width;
		m_height = _height;
	}
}