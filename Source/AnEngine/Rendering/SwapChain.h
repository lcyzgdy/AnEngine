#pragma once
#ifndef __ANENGINE__SWAPCHAIN_H__
#define __ANENGINE__SWAPCHAIN_H__

namespace AnEngine::Rendering
{
	class IPlatformSwapChain
	{
	};

	class SwapChain : IPlatformSwapChain
	{
	private:
		IPlatformSwapChain* m_impl;
	};
}

#endif // !__ANENGINE__SWAPCHAIN_H__
