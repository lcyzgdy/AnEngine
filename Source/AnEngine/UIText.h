#pragma once
#ifndef __UITEXT_H__
#define __UITEXT_H__

#include "UIComponent.h"
#include "Color.h"

namespace AnEngine::Game::UI
{
	class DLL_API UIText : public UIComponent
	{
		std::wstring m_text;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_textBrush;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;

	protected:
		// 通过 UIComponent 继承
		virtual void OnGUI() override;

	public:
		UIText();
		UIText(std::wstring&& text, Color&& color);
		UIText(const std::wstring& text, const Color& color);

		std::wstring Text();
		void Text(const std::wstring& text);
		void Text(std::wstring&& text);

		void ResetTextColor(const Color& color);
		void ResetTextColor(Color&& color);
#undef GLO
	};
}

#endif // !__UITEXT_H__
