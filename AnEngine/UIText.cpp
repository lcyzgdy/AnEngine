#include "UIText.h"
#include "GraphicsCard2D.h"
#include "RenderCore.h"

using namespace std;
using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::Game::UI
{
	UIText::UIText(wstring&& text, Color&& color) : m_text(text)
	{
	}

	UIText::UIText(const wstring& text, const Color& color)
	{
	}

	void UIText::OnGUI()
	{
		//D2D1_SIZE_F rtSize = m_d2dRenderTargets[r_frameIndex]->GetSize();
		//D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
		//context->SetTransform(D2D1::Matrix3x2F::Identity());
		//context->DrawText(m_text.c_str(), m_text.length(), m_textFormat.Get(), &textRect, m_textBrush.Get());
	}

	std::wstring UIText::Text()
	{
		return m_text;
	}

	void UIText::Text(const std::wstring& text)
	{
		m_text = text;
	}

	void UIText::Text(std::wstring && text)
	{
		m_text = text;
	}
}