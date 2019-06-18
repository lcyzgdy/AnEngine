#pragma once
#ifndef __SCREEN_H__
#define __SCREEN_H__
#include"onwind.h"
#include<atomic>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow);

namespace AnEngine
{
	class Screen : public NonCopyable//::Singleton<Screen>
	{
		// 2017.10.20
		// Friend functions only can be class's friend in same namespace. So I will redesign in the future
		// ************************************************************************************************
		//friend int WINAPI ::WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
		// */
		//friend void RenderCore::InitializeRender(int graphicCardCount, bool isStable);
		friend class Engine;
		//static Screen* m_uniqueObj;

		std::atomic<int> m_width;
		std::atomic<int> m_height;

		enum TargetResolution : int
		{
			S720P,
			S900P,
			S1080P,
			S1440P,
			S4K,
			S8K
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