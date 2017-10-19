#pragma once
#ifndef __SCREEN_H__
#define __SCREEN_H__
#include"onwind.h"
#include<atomic>

namespace AEngine
{
	class Screen : public NonCopyable
	{
		friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
		//friend void RenderCore::InitializeRender(int graphicCardCount, bool isStable);

		std::atomic<int> m_width;
		std::atomic<int> m_height;

		enum TargetResolution : int
		{
			S720P,
			S900P,
			S1080P,
			S1440P,
			S1800P,
			S2160P
		} TargetResolution;

		void Initialize(const int _width, const int _height);

	public:

		inline int Width()
		{
			return m_width;
		}

		inline int Height()
		{
			return m_height;
		}

		inline static Screen* GetInstance()
		{
			static Screen screen;
			return &screen;
		}
	};
}

#endif // !__SCREEN_H__