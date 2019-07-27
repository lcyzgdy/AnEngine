#include "UIText.h"
#include "GraphicsCard2D.h"
#include "RenderCore.h"

using namespace std;
using namespace AnEngine::RenderCore;


namespace AnEngine::Game::UI
{
	UIText::UIText()
	{
		r_graphicsCard2D->CreateTextFormat(&m_textFormat);
		ThrowIfFailed(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
		ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));

		r_graphicsCard2D->CreateTextBrush(&m_textBrush, Color::Black);
	}

	UIText::UIText(wstring&& text, Color&& color) : m_text(text)
	{
		r_graphicsCard2D->CreateTextFormat(&m_textFormat);
		ThrowIfFailed(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
		ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));

		r_graphicsCard2D->CreateTextBrush(&m_textBrush, color);
	}

	UIText::UIText(const wstring& text, const Color& color) : m_text(text)
	{
		r_graphicsCard2D->CreateTextFormat(&m_textFormat);
		ThrowIfFailed(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
		ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));

		r_graphicsCard2D->CreateTextBrush(&m_textBrush, color);
	}

	void UIText::OnGUI()
	{
		r_graphicsCard2D->DrawText1(m_text.c_str(), m_text.length(), m_textFormat.Get(), m_textBrush.Get());
	}

	std::wstring UIText::Text()
	{
		return m_text;
	}

	void UIText::Text(const std::wstring& text)
	{
		m_text = text;
	}

	void UIText::Text(std::wstring&& text)
	{
		m_text = text;
	}
}