#pragma once
#ifndef __SCREEN_H__
#define __SCREEN_H__
#include"onwind.h"
#include<atomic>

class Screen
{
	static atomic<int> width;
	static atomic<int> height;

public:

	static void InitializeScreen(const int _width, const int _height);

	inline static int Width()
	{
		return width;
	}

	inline static int Height()
	{
		return height;
	}
};


#endif // !__SCREEN_H__