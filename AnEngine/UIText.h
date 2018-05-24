#pragma once
#ifndef __UITEXT_H__
#define __UITEXT_H__

#include "UIComponent.h"

namespace AnEngine::Game::UI
{
	class UIText : public UIComponent
	{
		std::wstring m_text;
	protected:
		// 通过 UIComponent 继承
		virtual void OnGUI() override;

	public:
		std::wstring Text();
		void Text(const std::wstring& text);
		void Text(std::wstring&& text);
	};
}

#endif // !__UITEXT_H__
