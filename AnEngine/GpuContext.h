#pragma once
#ifndef __GPUCONTEXT_H__
#define __GPUCONTEXT_H__

#include "onwind.h"
#include "GraphicsCard.h"

namespace AnEngine::RenderCore
{
	class GraphicsContext : public Singleton<GraphicsContext>
	{
		friend class Singleton<GraphicsContext>;

		std::vector<std::shared_ptr<GraphicsCard>> m_graphicsCard;

		GraphicsContext() = default;

	public:
		__forceinline GraphicsCard* Default()
		{
			return m_graphicsCard[0].get();
		}
	};
}
#endif // !__GPUCONTEXT_H__
